import wavesim
import unittest

class TestBuildInfo(unittest.TestCase):
    def test_build_info(self):
        self.assertTrue(isinstance(wavesim.build_info, str))
        self.assertTrue(len(wavesim.build_info) > 10)

    def test_build_number(self):
        self.assertTrue(isinstance(wavesim.build_number, int))
        self.assertTrue(wavesim.build_number > 0)

    def test_build_host(self):
        self.assertTrue(isinstance(wavesim.build_host, str))
        self.assertTrue(len(wavesim.build_info) > 10)

    def test_build_time(self):
        from datetime import datetime
        self.assertTrue(isinstance(wavesim.build_time, str))
        try:
            datetime.strptime(wavesim.build_time, "%Y-%m-%dT%H:%M:%SZ")
        except:
            self.fail()

    def test_commit_info(self):
        self.assertTrue(isinstance(wavesim.commit_info, str))
        self.assertTrue(len(wavesim.commit_info) > 10)

    def test_compiler_info(self):
        self.assertTrue(isinstance(wavesim.compiler_info, str))
        self.assertTrue(len(wavesim.commit_info) > 1)

    def test_cmake_configuration(self):
        self.assertTrue(isinstance(wavesim.cmake_configuration, str))
        self.assertTrue(len(wavesim.cmake_configuration) > 10)

unittest.main()
