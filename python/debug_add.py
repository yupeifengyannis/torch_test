import torch
input1 = torch.rand(2,2, requires_grad=True)
input2 = torch.rand(2,2, requires_grad = True)
output = torch.add(input1, input2)
grad = torch.randn(2,2)
output.backward(grad)

