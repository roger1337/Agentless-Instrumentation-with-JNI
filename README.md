# Agentless Instrumentation with JNI
This project instantiates an Instrumentation object without the use of the Java Attach API through a JNI method.
It does this through creating a mock internal representation of a java agent.
I hacked this together within a few hours so please feel free to contribute, and star if this helped you.

Some code from OpenJDK is integrated throughout the project, such as the JPLIS structures.
