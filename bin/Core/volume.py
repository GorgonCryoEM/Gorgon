import logging
from libpycore import Volume as Vol
from libpycore import Fill

class Volume(Vol):
			
	def __getitem__(self, tup):
		i, j, k = tup
		return Vol.get(self, i, j, k)
		
	def __setitem__(self, tup, val):
		i, j, k = tup
		Vol.set(self, i, j, k, val)
