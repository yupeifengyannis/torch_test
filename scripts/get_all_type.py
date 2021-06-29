import os
import argparse

parser = argparse.ArgumentParser(description = "gen new aten function and method cpp file")
parser.add_argument("--install_dir",
                    help = "aten auto gen file directory",
                    action = "store_true")
options = parser.parse_args()

if options.install_dir is None:
    assert False, "please set pytorch build aten dir"
if os.path.exists(options.install_dir):
    assert False, "install dir {0} is not existed!"

TENSOR_METHOD_FILE = os.path.join(options.install_dir, 'core', 'TensorMethods.cpp')
TORCH_FUNCTION_FILE = os.path.join(options.install_dir, 'Functions.cpp')

if not os.path.exists(TENSOR_METHOD_FILE):
    assert False, "file {0} is not existed!".format(TENSOR_METHOD_FILE)
if not os.path.exists(TORCH_FUNCTION_FILE):
    assert False, "file {0} is not existed!".format(TORCH_FUNCTION_FILE)

class FileManager(object):
    def gen_gtest_file(self):
        pass
    def _parse_op_return_arg(self):
        pass
    def _parse_op_input_arg(self):
        pass
    def _hack_op_function(self):
        """
        hack every tensor method's op, insert a function at begin of op to
        get all input data of op, and insert  a function at end of op to
        get return data of op
        """
        pass
