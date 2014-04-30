{
  'variables': {
    'chromium_code': 1,
  },
  'targets': [
    {
      'target_name': 'canscope_device',
      'type': 'static_library',
      'dependencies': [
        '../third_party/chromium/base/base_bin.gyp:base_bin',
        '../third_party/chromium/base_ex/base_ex.gyp:base_ex',
        '../third_party/chromium/base/base_bin.gyp:base_prefs_bin',
        '../device/device.gyp:device',
      ],
      'include_dirs': [
        '..',
      ], 
      'sources': [
        'scoped_trace_event.h',
        'scoped_trace_event.cc',
        'device_errors.h',
        'device_errors.cc',
        'device_error_list.h',
        'async_task.h',
        'async_task.cc',
        'canscope_notification_types.h',
        
        'device/device_delegate.h',
        'device/device_delegate.cc',
        'device/device_base.h',
        'device/device_manager.h',
        'device/device_manager.cc',
        'device/device_base.h',
        'device/device_base.cc',
        'device/device_handle_base.h',
        'device/device_handle_base.cc',
        'device/scoped_device_lock.h',
        'device/scoped_device_lock.cc',
        'device/config_manager.h',
        'device/config_manager.cc',
        'device/config_util.h',
        'device/config_util.cc',
        'device/scoped_device_property_commit.h',
        'device/scoped_device_property_commit.cc',
        'device/data_collecter.h',
        'device/data_collecter.cc',
        'device/threaded_loop_run.h',
        'device/threaded_loop_run.cc',
        'device/devices_manager.h',
        'device/devices_manager.cc',
        'device/canscope_device_property_constants.h',
        'device/canscope_device_property_constants.cc',
        'device/canscope_device.h',
        'device/canscope_device.cc',
        'device/canscope_device_handle.h',
        'device/canscope_device_handle.cc',
        'device/canscope_device_finder.h',
        'device/canscope_device_finder.cc',
        'device/canscope_device_constants.h',
        'device/canscope_device_runner.h',
        'device/canscope_device_runner.cc',
        'device/sync_call.h',
        'device/sync_call.cc',
        'device/read_statistics.h',
        'device/read_statistics.cc',
        'device/usb_port_device_delegate.h',
        'device/usb_port_device_delegate.cc',
        
        'device/register/canscope_device_register_constants.h',
        'device/register/soft_diff_register.h',
        'device/register/soft_diff_register.cc',
        'device/register/analog_ctrl_register.h',
        'device/register/analog_ctrl_register.cc',
        'device/register/analog_switch_register.h',
        'device/register/analog_switch_register.cc',
        'device/register/trigger1_register.h',
        'device/register/trigger1_register.cc',
        'device/register/trigger2_register.h',
        'device/register/trigger2_register.cc',
        'device/register/trigger_state_register.h',
        'device/register/trigger_state_register.cc',
        'device/register/scope_ctrl_register.h',
        'device/register/scope_ctrl_register.cc',
        'device/register/device_info.h',
        'device/register/device_info.cc',
        'device/register/sja1000_register.h',
        'device/register/sja1000_register.cc',
        'device/register/frame_storage_register.h',
        'device/register/frame_storage_register.cc',
        'device/register/wave_storage_register.h',
        'device/register/wave_storage_register.cc',
        'device/register/wave_read_ctrl_register.h',
        'device/register/wave_read_ctrl_register.cc',
        'device/register/fpga_send_register.h',
        'device/register/fpga_send_register.cc',
        
        'device/osc_device/osc_device_property.h',
        'device/osc_device/osc_device_property.cc',
        'device/osc_device/osc_device.h',
        'device/osc_device/osc_device.cc',
        'device/osc_device/osc_device_handle.h',
        'device/osc_device/osc_device_handle.cc',
        'device/osc_device/osc_raw_data.h',
        'device/osc_device/osc_raw_data.cc',
        'device/osc_device/osc_data_collecter.h',
        'device/osc_device/osc_data_collecter.cc',
        
        'device/frame_device/frame_device.h',
        'device/frame_device/frame_device.cc',
        'device/frame_device/frame_device_property.h',
        'device/frame_device/frame_device_property.cc',
        'device/frame_device/frame_device_handle.h',
        'device/frame_device/frame_device_handle.cc',
        'device/frame_device/sja_btr_util.h',
        'device/frame_device/sja_btr_util.cc',
        'device/frame_device/frame_data_collecter.h',
        'device/frame_device/frame_data_collecter.cc',
        'device/frame_device/frame_raw_data.h',
        'device/frame_device/frame_raw_data.cc',
        
        'device/property/property.h',
        'device/property/property_delegate.h',
        
      ], 
    },
    
    {
      'target_name': 'canscope_usb_port',
      'type': 'static_library',
      'dependencies': [
        '../third_party/chromium/base/base_bin.gyp:base_bin',
      ],
      'include_dirs': [
        '..',
      ],
      'direct_dependent_settings': {      
        'msvs_settings':{
          'VCLinkerTool': {
            'AdditionalDependencies': [
              'setupapi.lib',
            ],
          },
        },
      },
      'sources': [
        'device/usb/usb_port.h',
        'device/usb/usb_port.cc',
        'device/usb/usb_command.h',
        'device/usb/usb_command.cc',
        'device/usb/usb_constants.h',
      ], 
    },
    {
      'target_name': 'canscope_device_unittests',
      'type': '<(gtest_target_type)',
      'sources': [
        'device/device_base_unittest.cc',
        'device/canscope_device_unittest.cc',
        'device/merge_device_list_unittest.cc',
        'device/devices_manager_unittest.cc',
        
        'device/register/soft_diff_register_unittest.cc',
        'device/register/analog_ctrl_register_unittest.cc',
        'device/register/analog_switch_register_unittest.cc',
        'device/register/trigger1_register_unittest.cc',
        'device/register/trigger2_register_unittest.cc',
        'device/register/trigger_state_register_unittest.cc',
        'device/register/scope_ctrl_register_unittest.cc',
        'device/register/sja1000_register_unittest.cc',
        'device/register/frame_storage_register_unittest.cc',
        'device/register/fpga_send_register_unittest.cc',
        
        'device/osc_device/osc_device_unittest.cc',
        
        'device/frame_device/frame_device_unittest.cc',
        
        'device/property/property_unittest.cc',
        
        'device/usb/usb_port_unittest.cc',
         
        'device/test/scoped_open_device.h',
        'device/test/scoped_open_device.cc',
        'test/test_process.h',
        'test/test_process.cc',
      ],
      
      'include_dirs': [
        '..',
      ], 
      'dependencies': [
        '../third_party/chromium/base/base_bin.gyp:base_bin',
        '../third_party/chromium/base/base_bin.gyp:base_prefs_bin',
        '../third_party/chromium/base/third_party/dynamic_annotations/dynamic_annotations.gyp:dynamic_annotations',
        '../third_party/chromium/base_ex/base_ex.gyp:base_ex',
        '../third_party/chromium/testing/gmock.gyp:gmock',
        '../third_party/chromium/testing/gtest.gyp:gtest',
        '../common/common.gyp:common',
        '../device/device.gyp:device',
        'canscope_usb_port',
        'canscope_device',
        'device_simulate/device_simulate.gyp:simulate_device',
        'canscope.gyp:test_support',
      ],
    },
    
    {
      'target_name': 'fpga',
      'type': 'none',

      'copies': [
        {
          'destination': '<(PRODUCT_DIR)/../Release/',
          'files': [
            'fpga/Release/CANScope.dll',
            'fpga/Release/CANScopePro.dll',
          ],
        },
        {
          'destination': '<(PRODUCT_DIR)/../Debug/',
          'files': [
            'fpga/Debug/CANScope.dll',
            'fpga/Debug/CANScopePro.dll',
          ],
        },
      ],   
    },
  ],
}