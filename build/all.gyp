{
  'targets': [
    {
      'target_name': 'all',
      'type': 'none',
      'dependencies': [
        '../device/device.gyp:*',
        '../canscope/canscope_device.gyp:*',
        '../depend_calc/depend_calc.gyp:*',
        '../canscope/canscope.gyp:*',
        '../wave_control/examples/examples.gyp:wave_control_example',
        '../wave_control/wave_control.gyp:*',
      ],
    }, # target_name: All
  ], # conditions
}