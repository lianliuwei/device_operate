{
  'type': 'executable',
  
  'variables': {
    'BASE_PATH': '../third_party/chromium/',
  },
  
  'dependencies': [
    '<(BASE_PATH)/base/base_bin.gyp:base_bin',
    '<(BASE_PATH)/base/base_bin.gyp:base_i18n_bin',
    '<(BASE_PATH)/skia/skia_bin.gyp:skia_bin',
    '<(BASE_PATH)/ui/ui_bin.gyp:ui_bin',
    '<(BASE_PATH)/ui/ui_bin.gyp:ui_resources_bin',
    '<(BASE_PATH)/chrome/chrome_resources_bin.gyp:packed_resources_bin',
    '<(BASE_PATH)/ui/views/views_bin.gyp:views_bin',
    '<(BASE_PATH)/third_party/icu_bin.gyp:icui18n_bin',
    '<(BASE_PATH)/third_party/icu_bin.gyp:icuuc_bin', '<(BASE_PATH)/base/third_party/dynamic_annotations/dynamic_annotations.gyp:dynamic_annotations',
  ],
  'include_dirs': [
   '..',
  ],

  'conditions': [
    ['OS=="win"', {
      'link_settings': {
        'libraries': [
          '-limm32.lib',
          '-loleacc.lib',
        ]
      },

      'msvs_settings': {
        'VCManifestTool': {
          'AdditionalManifestFiles': '$(ProjectDir)\\exe.manifest',
        },
      },
      'msvs_settings': {
        'VCLinkerTool': {
          #   1 == /SUBSYSTEM:CONSOLE
          'SubSystem': '1',
        },
      }, 
    }],
  ],
}