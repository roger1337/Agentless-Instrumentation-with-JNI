# Agentless Instrumentation with JNI
This project instantiates an Instrumentation object without the use of the Java Attach API through a JNI method.
It does this through creating a mock internal representation of a java agent.
I hacked this together within a few hours so please feel free to contribute, and star if this helped you.

Some code from OpenJDK is integrated throughout the project, such as the JPLIS structures.
Find the OpenJDK jdk here: https://github.com/openjdk/jdk

# How do I use this?

Compile the DLL in the dll directory (you have to use the /includes folder from a JDK to compile it)
Then create a class called InstrumentationManager with a native method getInstrumentation:

![image](https://github.com/roger1337/Agentless-Instrumentation-with-JNI/assets/85001442/e3c138cd-e40c-4c89-9266-9bda18ddb053)

This code has been tested on Java 1.8 only with the tests in the tests folder.




