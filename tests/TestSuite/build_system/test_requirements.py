import pkg_resources
import pkgutil
import unittest

dependencies = pkgutil.get_data(__package__, 'requirements.txt')


class Requirements(unittest.TestCase):

    def test(self):
        pkg_resources.require(dependencies)
