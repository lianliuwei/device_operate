{
  'variables': {
    'chromium_code': 1,
  },
  'targets': [
    {
      'target_name': 'osc_chnl_calc',
      'type': 'static_library',
      'dependencies': [
        '../third_party/chromium/base/base_bin.gyp:base_bin',
        '../third_party/chromium/base_ex/base_ex.gyp:base_ex',
        '../third_party/chromium/base/base_bin.gyp:base_prefs_bin',
      ],
      'include_dirs': [
        '..',
      ], 
      'sources': [
        'osc_chnl_calc/chnl_calc_manager.h',
        'osc_chnl_calc/chnl_calc_manager.cc',
        'osc_chnl_calc/chnl_calc_result.h',
        'osc_chnl_calc/chnl_calc_result.cc',
        'osc_chnl_calc/osc_chnl_data.h',
        'osc_chnl_calc/osc_chnl_data.cc',
        'osc_chnl_calc/chnl_calc_item.h',
        'osc_chnl_calc/chnl_calc_item.cc',
        'osc_chnl_calc/can_diff_calc_item.h',
        'osc_chnl_calc/can_diff_calc_item.cc',
        'osc_chnl_calc/freq_time.h',
        'osc_chnl_calc/freq_time.cc',
        'osc_chnl_calc/osc_chnl_data.h',
        'osc_chnl_calc/osc_chnl_data.cc',
      ], 
    },
    {
      'target_name': 'test_support',
      'type': 'static_library',
      'sources' : [
        'test/run_all_unittests.cc',
        'test/test_suite.h',
        'test/test_suite.cc',
        'test/speed_meter.h',
        'test/speed_meter.cc',
        'test/perftimer.h',
        'test/perftimer.cc',
        'test/scoped_trace_event.h',
        'test/scoped_trace_event.cc',
        'test/test_util.h',
        'test/test_util.cc',
      ],
      'include_dirs': [
        '..',
      ], 
       'dependencies': [
        '../third_party/chromium/base/base_bin.gyp:base_bin',
        '../third_party/chromium/testing/gtest.gyp:gtest',
       ],
    },
  ],
}