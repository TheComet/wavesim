import wavesim
import unittest

class TestAttribute(unittest.TestCase):
    def test_default(self):
        a = wavesim.Attribute()
        self.assertEqual(a.reflection, 0.0)
        self.assertEqual(a.transmission, 0.0)
        self.assertEqual(a.absorption, 1.0)

    def test_construct_with_values(self):
        a = wavesim.Attribute(1, 2, 3)
        self.assertEqual(a.reflection, 1.0)
        self.assertEqual(a.transmission, 2.0)
        self.assertEqual(a.absorption, 3.0)

    def test_construct_with_keyword_arg(self):
        a = wavesim.Attribute(transmission=1)
        self.assertEqual(a.reflection, 0.0)
        self.assertEqual(a.transmission, 1.0)
        self.assertEqual(a.absorption, 0.0)

    def test_can_set_attributes(self):
        a = wavesim.Attribute()
        a.reflection = 4;
        a.transmission = 5;
        a.absorption = 6;
        self.assertEqual(a.reflection, 4.0)
        self.assertEqual(a.transmission, 5.0)
        self.assertEqual(a.absorption, 6.0)

    def test_cant_delete_attributes(self):
        a = wavesim.Attribute()
        try:
            del a.reflection
            self.fail("Was able to delete reflection")
        except:
            pass

        try:
            del a.transmission
            self.fail("Was able to delete transmission")
        except:
            pass

        try:
            del a.absorption
            self.fail("Was able to delete absorption")
        except:
            pass

    def test_cant_set_attribute_to_something_else(self):
        a = wavesim.Attribute()
        with self.assertRaises(TypeError):
            a.reflection="lol"
        with self.assertRaises(TypeError):
            a.transmission="lol"
        with self.assertRaises(TypeError):
            a.absorption="lol"

unittest.main()
