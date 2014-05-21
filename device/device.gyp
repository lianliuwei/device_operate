{
  'variables': {
    'chromium_code': 1,
    'BASE_PATH': '../third_party/chromium/',
  },
  'targets': [
    {
      'target_name': 'device',
      'type': 'static_library',
      'dependencies': [
        '<(BASE_PATH)/base/base_bin.gyp:base_bin',
        '<(BASE_PATH)/base_ex/base_ex.gyp:base_ex',
      ],
      'include_dirs': [
        '..',
      ], 
      'sources': [
        'bool_bind.h',
        'one_byte_type_bind.h',
        'simple_type_bind.h',
        'uint16_split_bind.h',
        'uint16_split_bind.cc',
        'simple_type_bind.h',
        'int_bind.h',
        'uint32_sub_bind.h',
        'uint32_sub_bind.cc',
        'endian.h',
        'register_group.h',
        'register_memory.h',
        'register_memory.cc',
        'register_property.h',
        'memory_frame_pool.h',
        'memory_frame_pool.cc',
        'sequenced_bulk_queue.h',
        'sequenced_bulk_queue.cc',
        'memory_usage_pool.h',
        'memory_usage_pool.cc',
        'property/property_notifiter.h',
        'property/property_notifiter.cc',
        'property/store_member.h',
        'property/store_member.cc',
        'property/enum_store_member.h',
        'property/device_property_store.h',
        'property/value_map_device_property_store.h',
        'property/value_map_device_property_store.cc',
  
      ], 
    },
    {
      'target_name': 'device_unittests',
      'type': '<(gtest_target_type)',
      'sources': [
        'bool_bind_unittest.cc',
        'uint8_bind_unittest.cc',
        'uint16_split_bind_unittest.cc',
        'register_memory_unittest.cc',
        'simple_type_bind_unittest.cc',
        'uint32_sub_bind_unittest.cc',
        'memory_usage_pool_unittest.cc',
        'memory_frame_pool_unittest.cc',
        'sequenced_bulk_queue_unittest.cc',
        'property/device_property_observer_mock.h',
        'property/enum_store_member_unittest.cc',
        'property/store_member_unittest.cc',
        'property/value_map_device_property_store_unittest.cc',
      ],
      
      'include_dirs': [
        '..',
      ], 
      'dependencies': [
        '../test/test.gyp:test_support',
        '<(BASE_PATH)/base/base_bin.gyp:base_bin',
        '<(BASE_PATH)/base/base_bin.gyp:base_prefs_bin',
        '<(BASE_PATH)/base/third_party/dynamic_annotations/dynamic_annotations.gyp:dynamic_annotations',
        '<(BASE_PATH)/base_ex/base_ex.gyp:base_ex',
        '<(BASE_PATH)/testing/gmock.gyp:gmock',
        '<(BASE_PATH)/testing/gtest.gyp:gtest',
        'device',
      ],
    },
  ],
}