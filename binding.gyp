{
  'targets': [
    {
      'target_name': 'notify-native',
      'conditions': [
        ['OS=="win"', {
          'sources': [ 'src/notify.cc', 'WinToast/src/wintoastlib.cpp' ],
          'include_dirs': ["<!@(node -p \"require('node-addon-api').include\")", "<!@(node -p \"require('napi-thread-safe-callback').include\")", "WinToast/src"],
          'dependencies': ["<!(node -p \"require('node-addon-api').gyp\")"],
          'msvs_settings': {
            'VCCLCompilerTool': { 'ExceptionHandling': 1 },
          },
          'defines': [ '_HAS_EXCEPTIONS=1' ]
        }]
	    ]
    }
  ]
}
