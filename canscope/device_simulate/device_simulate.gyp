{
  'variables': {
    'chromium_code': 1,
    'BASE_PATH': '../../third_party/chromium/',
  },
  'targets': [
    {
      'target_name': 'simulate_device',
      'type': 'static_library',
      'dependencies': [
        '<(BASE_PATH)/base/base_bin.gyp:base_bin',
        '<(BASE_PATH)/base_ex/base_ex.gyp:base_ex',
      ],
      
      'include_dirs': [
        '../..',
      ], 
      'sources': [
        'simulate_device_delegate.h',
        'simulate_device_delegate.cc',
        'canscope_device_register_group.h',
        'canscope_device_register_group.cc',
        'osc_device_simulate.h',
        'osc_device_simulate.cc',
      ], 
    },
  ],
}