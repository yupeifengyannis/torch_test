# pytorch中的序列化操作

pytorch中对于Tensor、Scalar、TensorOptions等等对象的序列化操作主要是通过IValue+pickle进行操作的，比如下面这个简单的例子。

