{
  'targets': [
    {
      'target_name': 'copy_vld_file',
      'type': 'none',

      'copies': [
        {
          'destination': '<(PRODUCT_DIR)',
          'files': [
            'vld.ini',
            'bin/Win32/dbghelp.dll',
            'bin/Win32/vld_x86.dll',
          ],
        },
        {
          'destination': '<(PRODUCT_DIR)/lib/',
          'files': [
            'lib/Win32/vld.lib',
          ],
        },
      ],   
    },
    
    {
      'target_name': 'vld',
      'type': 'none',
      'sources':[
        'include/vld.h',
        'include/vld.cc',
      ],
      'direct_dependent_settings': { 
        'sources':[
          'include/vld.cc',
        ],
        'msvs_settings':{
          'VCLinkerTool': {
            'AdditionalDependencies': [
              'vld.lib',
            ],
          },
        },
      },
    },
    
    {
      'target_name': 'vld_sample',
      'type': 'executable',
   
      'msvs_settings':{
        'VCLinkerTool': {
          'AdditionalLibraryDirectories': [
            '$(OutDir)/lib',
          ],
        },
      },
      
      'sources' : [
        'sample/main.cc',
      ],
      
      'dependencies': [
        'vld',
      ],
    }
  ],
}