project('webfuse-provider', version: '0.1.0', license: 'LGPL-3.0+')


webfuse_provider_dep = dependency('webfuse_provider', version: '>=0.3.0',
    fallback: ['webfuse', 'webfuse_provider_dep'], default_options: ['without_adapter=true'])

executable('webfuse-provider',
    'src/main.c',
    dependencies: [webfuse_provider_dep])

executable('static-filesystem-provider',
	'src/static_filesystem/static_filesystem.c',
	'src/static_filesystem/path.c',
	'src/static_filesystem/main.c',
    dependencies: [webfuse_provider_dep])
