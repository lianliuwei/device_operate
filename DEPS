vars = {
  'git_url':
    'http://chromium.googlesource.com',
  'self_url':
    'http://github.com/lianliuwei',
}

deps = {
  'third_party/chromium/':
      Var('self_url') + 
      '/chromium_base2.git@d5b865a99bfac89a3d880c942dd3a534c8f3d754',
  # ui need those head    
  'third_party/chromium/third_party/skia/include':
      Var('git_url') + '/external/skia/include.git@44dd501fd86752e61a0291fec7ca7d3b51d64563',
      
  'third_party/chromium/testing/gmock':
      Var('git_url') + '/external/googlemock.git@6b1759c3816d574bddde3e1725c51a811c8870e7',
  'third_party/chromium/testing/gtest':
      Var('git_url') + '/external/googletest.git@a6772271f71672e889776bfe49ec4efd9da036df',
    
  'tools/gyp':
      Var('git_url') + '/external/gyp.git@a550dd22b6c28bb1a16fe9d406030a0f755ad79d',
}