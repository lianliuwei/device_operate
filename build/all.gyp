{
  'targets': [
    {
      'target_name': 'all',
      'type': 'none',
      'dependencies': [
        '../device/device.gyp:*',
        '../canscope/canscope_device.gyp:*',
      ],
    }, # target_name: All
  ], # conditions
}