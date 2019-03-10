from setuptools import setup

setup(
	name='tartu_icpc',
	version='0.1',
	description='For use in Tartu ICPC team codebook',
	packages=['tartu_icpc'],
	zip_safe=False,
	entry_points = {
		'pygments.lexers': [
			'tartu_icpc = tartu_icpc.pygments:TartuICPCLexer',
		],
		'pygments.styles': [
			'tartu_icpc = tartu_icpc.pygments:TartuICPCStyle',
		],
	},
)
