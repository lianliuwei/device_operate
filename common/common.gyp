{
  'variables': {
    'chromium_code': 1,
  },
  'targets': [
    {
      'target_name': 'common',
      'type': 'static_library',
      'dependencies': [
        '../third_party/chromium/base/base_bin.gyp:base_bin',
        '../third_party/chromium/base/third_party/dynamic_annotations/dynamic_annotations.gyp:dynamic_annotations',
      ],      
      'include_dirs': [
        '..',
      ],   
      'defines': [
        'COMMON_IMPLEMENTATION',
      ],
      'sources': [
        'common_export.h',
        'common_thread.h',
        'common_thread_delegate.h',
        'common_thread_impl.h',
        'common_thread_impl.cc',
        'common_thread_manager.h',
        'common_thread_manager.cc',
        'notification/notification_details.h',
        'notification/notification_observer.h',
        'notification/notification_registrar.cc',
        'notification/notification_registrar.h',
        'notification/notification_service.h',
        'notification/notification_source.h',
        'notification/notification_types.h',
        'notification/notification_service_impl.h',
        'notification/notification_service_impl.cc',
      ],
    },   
  ],
}