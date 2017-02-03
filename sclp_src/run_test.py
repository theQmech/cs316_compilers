import unittest, os, filecmp


class TestTokens(unittest.TestCase):
    pass

def init_test(i):
    os.system("./sclp -tokens -ast testcases/test{0:02d}.c 2>testcases/err".format(i))
    os.system("mv testcases/test{0:02d}.c.ast testcases/ast".format(i))
    os.system("mv testcases/test{0:02d}.c.spim testcases/spim".format(i))
    os.system("sed 's/[0-9]\+$//' testcases/test{0:02d}.c.toks >testcases/toks".format(i))
    os.system("./sclp16 -tokens -ast testcases/test{0:02d}.c 2>testcases/err_ref".format(i))
    os.system("mv testcases/test{0:02d}.c.ast testcases/ast_ref".format(i))
    os.system("mv testcases/test{0:02d}.c.spim testcases/spim_ref".format(i))
    os.system("sed 's/[0-9]\+$//' testcases/test{0:02d}.c.toks >testcases/toks_ref".format(i))
    os.system("touch err err_ref")

def create_err_test(i):
    def do_test_toks(self):
        init_test(i)
        self.assertTrue(filecmp.cmp("testcases/err", "testcases/err_ref"))
    return do_test_toks

def create_toks_test(i):
    def do_test_toks(self):
        init_test(i)
        self.assertTrue(filecmp.cmp("testcases/toks", "testcases/toks_ref"))
    return do_test_toks

def create_ast_test(i):
    def do_test_ast(self):
        init_test(i)
        self.assertTrue(filecmp.cmp("testcases/ast", "testcases/ast_ref"))
    return do_test_ast

def create_mips_test(i):
    def do_test_ast(self):
        init_test(i)
        self.assertTrue(filecmp.cmp("testcases/ast", "testcases/ast_ref"))
    return do_test_ast

if __name__ == '__main__':
    for i in range(1,50):

        toks_test = create_toks_test(i)
        toks_test.__name__ = 'test_%d_toks' % i
        setattr (TestTokens, toks_test.__name__, toks_test)

        ast_test = create_ast_test(i)
        ast_test.__name__ = "test_%d_ast" %i
        setattr (TestTokens, ast_test.__name__, ast_test)

        mips_test = create_mips_test(i)
        mips_test.__name__ = "test_%d_mips" %i
        setattr (TestTokens, mips_test.__name__, mips_test)

        err_test = create_err_test(i)
        err_test.__name__ = "test_%d_err" %i
        setattr (TestTokens, err_test.__name__, err_test)

    unittest.main(verbosity=2)
