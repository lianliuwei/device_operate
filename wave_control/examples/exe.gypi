{
  'type': 'executable',
  'dependencies': [
    '<(DEPTH)/base/base.gyp:base',
    '<(DEPTH)/base/base.gyp:base_i18n',
    '<(DEPTH)/skia/skia.gyp:skia',
    '<(DEPTH)/ui/ui.gyp:ui',
    '<(DEPTH)/ui/ui.gyp:ui_resources',
    '<(DEPTH)/chrome/chrome_resources.gyp:packed_resources',
    '<(DEPTH)/ui/views/views.gyp:views',
    '<(DEPTH)/ui/compositor/compositor.gyp:compositor',
    '<(DEPTH)/third_party/icu/icu.gyp:icui18n',
    '<(DEPTH)/third_party/icu/icu.gyp:icuuc', '<(DEPTH)/base/third_party/dynamic_annotations/dynamic_annotations.gyp:dynamic_annotations',
  ],
  'include_dirs': [
   '../..',
  ],

  'conditions': [
    ['OS=="win"', {
      'link_settings': {
        'libraries': [
          '-limm32.lib',
          '-loleacc.lib',
        ]
      },
      'include_dirs': [
        '<(DEPTH)/third_party/wtl/include',
      ],
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