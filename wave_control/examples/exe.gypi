{
  'type': 'executable',
  'dependencies': [
    '<(DEPTH)/base/base.gyp:base',
    '<(DEPTH)/skia/skia.gyp:skia',
    '<(DEPTH)/ui/ui.gyp:ui',
    '<(DEPTH)/ui/ui.gyp:ui_resources',
    '<(DEPTH)/chrome/chrome_resources.gyp:packed_resources',
    '<(DEPTH)/ui/views/views.gyp:views',
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
          #   2 == /SUBSYSTEM:CONSOLE
          'SubSystem': '2',
        },
      }, 
    }],
  ],
}