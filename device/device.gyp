{
  'variables': {
    'chromium_code': 1,
  },
  'targets': [
    {
      'target_name': 'device',
      'type': 'executable',
      'variables': {
        'mfc_target': 1,
      },
      'dependencies': [
        '../third_party/chromium/base/base_bin.gyp:base_bin',
        '../third_party/chromium/base_ex/base_ex.gyp:base_ex',
      ],
      'include_dirs': [
        '..',
      ], 
      'sources': [
        'bool_bind.h',
        'canscope_device.h',
        'canscope_device_constants.h',
        'canscope_osc_device.h',
        'canscope_osc_device2.h',
        'device_errors.h',
        'endian.h',
        'int_bind.h',
        'register_group.h',
        'register_memory.h',
        'register_memory.cc',
        'register_property.h',
      ], 
    },
  ],
}