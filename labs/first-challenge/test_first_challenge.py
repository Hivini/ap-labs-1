import first_challenge
import unittest

class TestFirstChallenge(unittest.TestCase):

    def test_no_elements(self):
        lst = []
        self.assertEqual(first_challenge.get_length(lst), 0, "Should be 0")

    def test_normal_array(self):
        lst = [1, 2, 3]
        self.assertEqual(first_challenge.get_length(lst), 3, "Should be 3")

    def test_nested_array(self):
        lst = [1, [2, 3], [4, [5, [6, 7, [8], 9]], 10]]
        self.assertEqual(first_challenge.get_length(lst), 10, "Should be 10")

if __name__ == '__main__':
    unittest.main()
