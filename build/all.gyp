{
  'targets': [
    {
      'target_name': 'all',
      'type': 'none',
      'dependencies': [
        '../device/device.gyp:*',
        '../canscope/canscope_device.gyp:*',
        '../depend_calc/depend_calc.gyp:*',
      ],
    }, # target_name: All
  ], # conditions
}