python aten/src/ATen/gen.py \
  --source-path aten/src/ATen \
  --install_dir build/aten/src/ATen \
  aten/src/ATen/Declarations.cwrap \
  aten/src/THCUNN/generic/THCUNN.h \
  aten/src/ATen/nn.yaml \
  aten/src/ATen/native/native_functions.yaml \
  --output-dependencies build/aten/src/ATen/generated_cpp.txt
