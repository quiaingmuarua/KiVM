编译java
```shell

cd java-src
javac -source 8 -target 8 -encoding utf-8 \
-bootclasspath ../rt.jar \
-d ../java-out \
com/imkiva/kivm/*.java
```


```shell

 ./java -cp "./rt.jar:./java-out" HelloKiVM
 ./java -cp "./rt.jar:./java-out" com.imkiva.kivm.HelloWorld
 
```


```shell
javac -source 8 -target 8 -encoding utf-8 -d java-out HelloKiVM.java

```