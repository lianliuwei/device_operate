{
  'targets' : [
    {
      'target_name': 'wave_control',
      'type': 'static_library',
      'dependencies': [
        '<(DEPTH)/base/base.gyp:base',
        '<(DEPTH)/ui/ui.gyp:ui',
        '<(DEPTH)/ui/ui.gyp:ui_resources',
        '<(DEPTH)/ui/views/views.gyp:views',
        '<(DEPTH)/skia/skia.gyp:skia',
      ],
      'sources': [
        # view model
        'wave_control.h',
        'wave_control.cc',
        'wave_container.h',
        'wave_container.cc',
        'yt_wave_container.h',
        'yt_wave_container.cc',
        'yl_wave_container.h',
        'xy_wave_container.h',
        'wave.h',
        'osc_wave.h',
        'osc_wave.cc',
        'osc_wave_observer.h',
        'simple_ana_wave.h',
        'simple_ana_wave.cc',
        'simple_ana_wave_observer.h',
        'simple_digit_wave.h',
        
        # view model visitor
        'wave_control_visitor.h',
        'wave_container_visitor.h',
        'wave_visitor.h',
        
        # wave data
        'ana_wave_data.h',
        'ana_wave_data.cc',
        'wave_range.h',
        'wave_range.cc',
        
        'wave_view.h',
        # model impl
        'default_simple_ana_wave.h',
        'default_simple_ana_wave.cc',
        
        
        # osc wave grup
        'osc_wave_group/osc_wave_group.h',
        'osc_wave_group/osc_wave_group.cc',
        'osc_wave_group/trigger_part.h',
        'osc_wave_group/horizontal_part.h',
        'osc_wave_group/vertical_part.h',
        'osc_wave_group/osc_wave_group_observer.h',
        
        # osc wave group impl
        'osc_wave_group/common_osc_wave_group.h',
        'osc_wave_group/common_osc_wave_group.cc',
        
        # views model match view
        'views/wave_control_view_factory.h',
        'views/wave_control_view_factory.cc',
        'views/wave_control_view.h',
        'views/wave_control_view.cc',
        'views/yt_wave_container_view.h',
        'views/yt_wave_container_view.cc',
        'views/osc_wave_view.h',
        'views/osc_wave_view.cc',
        'views/simple_ana_wave_view.h',
        'views/simple_ana_wave_view.cc',
        
        # views impl
        'views/yt_wave_container_inner_view.h',
        'views/yt_wave_container_inner_view.cc',
        'views/axis_background.h',
        'views/axis_background.cc',
        'views/axis_painter.h',
        'views/axis_painter.cc',
        'views/handle.h',
        'views/handle.cc',
        'views/handle_bar.h',
        'views/handle_bar.cc',
        'views/handle_bar_observer.h',
        'views/handle_bar_model.h',
        'views/handle_bar_model_observer.h',
        'views/ana_wave_data_view.h',
        'views/ana_wave_data_view.cc',
        'views/transform_util.h',
        'views/transform_util.cc',
        'views/wave_control_views_constants.h',
        'views/fill_box_layout.h',
        'views/fill_box_layout.cc',
        'views/all_fill_layout.h',
        'views/all_fill_layout.cc',
        'views/handle_point_view.h',
        'views/handle_point_view.cc',
        'views/measure_line/measure_line.h',
        'views/measure_line/dual_line_view.h',
        'views/measure_line/dual_line_view.cc',
        'views/measure_line/single_line_view.h',
        'views/measure_line/single_line_view.cc',
        'views/measure_line/measure_line_container_view.h',
        'views/measure_line/measure_line_container_view.cc',
        'views/measure_line/measure_line_part_view.h',
        'views/measure_line/measure_line_part_view.cc',
      ],
    }, 
 ],
}