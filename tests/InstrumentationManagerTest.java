import java.lang.instrument.ClassDefinition;
import java.lang.instrument.Instrumentation;


public class InstrumentationManagerTest {

    public static void main(String[] args) {
        try {
            System.load("...\\Get-Instrumentation JNI.dll");

            Instrumentation inst = InstrumentationManager.getInstrumentation();
            assert inst != null : "Instrumentation is null";
            assert inst.isRedefineClassesSupported() : "Redefine supported is not true";
            assert inst.isRetransformClassesSupported() : "Re-transform supported is not true";
            assert inst.isNativeMethodPrefixSupported() : "native prefix supported is not true";
            assert inst.isModifiableClass(TransformTestClass.class) : "TransformTestClass not modifiable";
            assert inst.getAllLoadedClasses().length > 0  : "Loaded class list empty";
            assert inst.getAllLoadedClasses()[0] != null : "Class at index 0 of loaded class list is null";

            ClassFileTransformerTest test = new ClassFileTransformerTest();
            inst.addTransformer(test, true);
            assert !inst.removeTransformer(new ClassFileTransformerTest()) : "Remove transformer on random object succeeded for some reason";

            try {
                inst.redefineClasses(new ClassDefinition(TransformTestClass.class, new byte[]{}));
                throw new Exception("empty byte array did not err on redefinition");
            } catch (ClassFormatError e) {
                // good
            }

            inst.retransformClasses(TransformTestClass.class); // Should print transforming TransformTestClass
            assert inst.removeTransformer(test) : "Remove transformer failed";

        } catch (Throwable e) {
            e.printStackTrace();
        }
    }
}
