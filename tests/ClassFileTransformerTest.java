import java.lang.instrument.ClassFileTransformer;
import java.lang.instrument.IllegalClassFormatException;
import java.security.ProtectionDomain;

public class ClassFileTransformerTest implements ClassFileTransformer {
   public byte[] transform(ClassLoader loader, String className, Class<?> classBeingRedefined, ProtectionDomain protectionDomain, byte[] classfileBuffer) {
       System.out.println("Transforming class: " + className + " with size: " + classfileBuffer.length);
       return classfileBuffer;
   }
}
