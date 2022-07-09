#
#  A Script for running from PyInstaller. Usage:
#
#     PyInstaller tyrant.spec
#
#


# -*- mode: python ; coding: utf-8 -*-
import sys


block_cipher = None


def get_version():

    #
    # Read and parse the version string in file "main.py". Because this is being
    # called from PyInstaller, we need a somewhat round-about way of finding and
    # importing "main.py".
    #
    # Returns version as a 4-tuple ("Microsoft-style" version)
    #

    import os
    import os.path
    import importlib
    import importlib.util
    spec = importlib.util.spec_from_file_location("main", os.path.join(os.getcwd(), "python", "main.py"))
    main = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(main)
    ver_strs = main.__version__.split(".")
    return (int(ver_strs[0]), int(ver_strs[1]), int(ver_strs[2]), 0)


ver = get_version()


if sys.platform.startswith('win'):
    
    # Create a version structure for windows executable.
    
    from PyInstaller.utils.win32.versioninfo import VSVersionInfo, FixedFileInfo, StringFileInfo, StringTable, StringStruct, VarFileInfo, VarStruct

    version_info_ = VSVersionInfo(
                      ffi=FixedFileInfo(
                        filevers=ver,
                        prodvers=ver,
                        mask=0x3f,
                        flags=0x0,
                        OS=0x04,
                        fileType=0x1,
                        subtype=0x0,
                        date=(0, 0)
                        ),
                      kids=[
                        StringFileInfo(
                          [
                          StringTable(
                            '000004B0',
                            [StringStruct('CompanyName', 'https://github.com/michgz'),
                            StringStruct('FileDescription', 'Tone editor for Casio keyboards'),
                            StringStruct('FileVersion', '{0}.{1}.{2}.{3}'.format(*ver)),
                            StringStruct('OriginalFilename', 'tyrant.exe'),
                            StringStruct('InternalName', 'tyrant'),
                            StringStruct('ProductName', 'ToneTyrant'),
                            StringStruct('ProductVersion', '{0}.{1}.{2}.{3}'.format(*ver))])
                          ]), 
                        VarFileInfo([VarStruct('Translation', [0, 1200])])
                      ]
                    )


    # Include the Windows SDK libraries. This is for the benefit of Windows 7, which
    # doesn't always have these available. From Windows 10 they are always available
    # (see https://blogs.msdn.microsoft.com/vcblog/2015/03/03/introducing-the-universal-crt/)
    #
    # -- FOR NOW, DON'T INCLUDE. --
    ##binaries_ = [ ("C:/Program Files (x86)/Windows Kits/10/Redist/ucrt/DLLs", "DLLs") ]
    binaries_ = []

else:

    # None of the above is relevant for Linux

    version_info_ = None
    binaries_ = []


a = Analysis(['python/main.py'],
             pathex=[],
             binaries=binaries_,
             datas=[ ('./tyrant-64x64.ico', '.') ],
             hiddenimports=[],
             hookspath=[],
             hooksconfig={},
             runtime_hooks=[],
             excludes=[],
             win_no_prefer_redirects=False,
             win_private_assemblies=False,
             cipher=block_cipher,
             noarchive=False)
pyz = PYZ(a.pure, a.zipped_data,
             cipher=block_cipher)

exe = EXE(pyz,
          a.scripts,
          a.binaries,
          a.zipfiles,
          a.datas,  
          [],
          name='tyrant',
          debug=False,
          bootloader_ignore_signals=False,
          strip=sys.platform.startswith('linux'),  # Significantly reduces binary size for linux
          upx=True,
          upx_exclude=[],
          runtime_tmpdir=None,
          console=False,
          disable_windowed_traceback=False,
          target_arch=None,
          codesign_identity=None,
          entitlements_file=None,
          icon='tyrant-64x64.ico',
          version=version_info_)
