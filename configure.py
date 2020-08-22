# vim: set ts=2 sw=2 tw=99 noet:
import sys
try:
	from ambuild2 import run
except:
	try:
		import ambuild
		sys.stderr.write('It looks like you have AMBuild 1 installed, but this project uses AMBuild 2.\n')
		sys.stderr.write('Upgrade to the latest version of AMBuild to continue.\n')
	except:
		sys.stderr.write('AMBuild must be installed to build this project.\n')
		sys.stderr.write('http://www.alliedmods.net/ambuild\n')
	sys.exit(1)

# Hack to show a decent upgrade message, which wasn't done until 2.2.
ambuild_version = getattr(run, 'CURRENT_API', '2.1')
if ambuild_version.startswith('2.1'):
	sys.stderr.write("AMBuild 2.2 or higher is required; please update\n")
	sys.exit(1)

run = run.BuildParser(sourcePath=sys.path[0], api='2.2')
run.options.add_argument('--hl2sdk-root', type=str, dest='hl2sdk_root', default=None,
		                   help='Root search folder for HL2SDKs')
run.options.add_argument('--mms-path', type=str, dest='mms_path', default=None,
                       help='Path to Metamod:Source')
run.options.add_argument('--sm-path', type=str, dest='sm_path', default=None,
                       help='Path to Sourcemod')
run.options.add_argument('--steamworks-path', type=str, dest='steamworks_path', default=None,
                       help='Path to SteamWorks')
run.options.add_argument('--enable-debug', action='store_const', const='1', dest='debug',
                       help='Enable debugging symbols')
run.options.add_argument('--enable-optimize', action='store_const', const='1', dest='opt',
                       help='Enable optimization')
run.options.add_argument('-s', '--sdks', default='all', dest='sdks',
                       help='Build against specified SDKs; valid args are "all", "present", or '
                            'comma-delimited list of engine names')
run.options.add_argument('--target', default=None, help='Override the default build target')
run.Configure()
