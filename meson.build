project('webfuse-provider', 'c', 'cpp', version: '0.4.0', license: 'LGPL-3.0+')

without_tests    = get_option('without_tests')
without_examples = get_option('without_examples')

libwebsockets_dep = dependency('libwebsockets', version: '>=4.0.13', required: false)
if not libwebsockets_dep.found()
	cmake = import('cmake')
	libwebsockets = cmake.subproject('libwebsockets')
	libwebsockets_dep = libwebsockets.dependency('websockets_shared')
endif

jansson_dep = dependency('jansson', version: '>=2.11', fallback: ['jansson', 'jansson_dep'])

pkg_config = import('pkgconfig')

# Webfuse provider

webfuse_provider_static = static_library('webfuse_provider',
    'lib/webfuse_provider/impl/slist.c',
	'lib/webfuse_provider/impl/message.c',
	'lib/webfuse_provider/impl/message_queue.c',
	'lib/webfuse_provider/impl/status.c',
	'lib/webfuse_provider/impl/base64.c',
	'lib/webfuse_provider/impl/lws_log.c',
	'lib/webfuse_provider/impl/json_util.c',
	'lib/webfuse_provider/impl/url.c',
    'lib/webfuse_provider/impl/timer/manager.c',
    'lib/webfuse_provider/impl/timer/timepoint.c',
    'lib/webfuse_provider/impl/timer/timer.c',
	'lib/webfuse_provider/impl/jsonrpc/proxy.c',
	'lib/webfuse_provider/impl/jsonrpc/proxy_variadic.c',
	'lib/webfuse_provider/impl/jsonrpc/server.c',
	'lib/webfuse_provider/impl/jsonrpc/method.c',
	'lib/webfuse_provider/impl/jsonrpc/request.c',
	'lib/webfuse_provider/impl/jsonrpc/response.c',
	'lib/webfuse_provider/impl/jsonrpc/error.c',
	'lib/webfuse_provider/api.c',
	'lib/webfuse_provider/impl/client.c',
	'lib/webfuse_provider/impl/client_config.c',
	'lib/webfuse_provider/impl/client_protocol.c',
	'lib/webfuse_provider/impl/provider.c',
	'lib/webfuse_provider/impl/request.c',
	'lib/webfuse_provider/impl/dirbuffer.c',
	'lib/webfuse_provider/impl/credentials.c',
	'lib/webfuse_provider/impl/operation/lookup.c',
	'lib/webfuse_provider/impl/operation/getattr.c',
	'lib/webfuse_provider/impl/operation/readdir.c',
	'lib/webfuse_provider/impl/operation/open.c',
	'lib/webfuse_provider/impl/operation/close.c',
	'lib/webfuse_provider/impl/operation/read.c',
    c_args: ['-fvisibility=hidden'],
    include_directories: ['include', 'lib'],
    dependencies: [libwebsockets_dep, jansson_dep])

webfuse_provider_static_dep = declare_dependency(
    include_directories: ['include'],
    link_with: webfuse_provider_static,
	dependencies: [libwebsockets_dep, jansson_dep])

webfuse_provider = shared_library('webfuse_provider',
    'lib/webfuse_provider/api.c',
    version: meson.project_version(),
    c_args: ['-fvisibility=hidden', '-DWFP_API=WFP_EXPORT'],
    include_directories: ['include', 'lib'],
    dependencies: [webfuse_provider_static_dep, libwebsockets_dep, jansson_dep],
	install: true)

webfuse_provider_dep = declare_dependency(
	include_directories: ['include'],
	link_with: [webfuse_provider],
	dependencies: [libwebsockets_dep, jansson_dep])

install_subdir('include/webfuse_provider', install_dir: 'include')

pkg_config.generate(
    libraries: [webfuse_provider],
	requires: ['libwebsockets', 'jansson'],
    subdirs: '.',
    version: meson.project_version(),
    name: 'libwebfuse_provider',
    filebase: 'webfuse_provider',
    description: 'Provider library for websockets filesystem')


# Unit Tests

if not without_tests

gtest_dep = dependency('gtest', version: '>=1.10.0', fallback: ['gtest', 'gtest_dep']) 
gmock_main_dep = dependency('gmock_main', version: '>=1.10.0', fallback: ['gtest', 'gmock_main_dep'])

fscheck = executable('fs_check',
	'test/webfuse_provider/tests/integration/fs_check.c')

openssl = find_program('openssl')
test_server_certs = custom_target('test_server_certs',
	output: ['server-key.pem', 'server-cert.pem'],
	command: [openssl, 'req', '-x509', '-newkey', 'rsa:4096', '-keyout', 'server-key.pem', '-out', 'server-cert.pem', '-days', '365', '-nodes', '-batch', '-subj', '/CN=localhost'])
test_client_certs = custom_target('test_client_certs',
	output: ['client-key.pem', 'client-cert.pem'],
	command: [openssl, 'req', '-x509', '-newkey', 'rsa:4096', '-keyout', 'client-key.pem', '-out', 'client-cert.pem', '-days', '365', '-nodes', '-batch', '-subj', '/CN=localhost'])

test_certs_dep = declare_dependency(
	sources: [test_server_certs, test_client_certs])

alltests = executable('alltests',
	'test/webfuse_provider/tests/core/jsonrpc/mock_timer_callback.cc',
	'test/webfuse_provider/tests/core/jsonrpc/mock_timer.cc',
	'test/webfuse_provider/tests/core/jsonrpc/test_is_request.cc',
	'test/webfuse_provider/tests/core/jsonrpc/test_request.cc',
	'test/webfuse_provider/tests/core/jsonrpc/test_is_response.cc',
	'test/webfuse_provider/tests/core/jsonrpc/test_response.cc',
	'test/webfuse_provider/tests/core/jsonrpc/test_server.cc',
	'test/webfuse_provider/tests/core/jsonrpc/test_proxy.cc',
	'test/webfuse_provider/tests/core/jsonrpc/test_response_parser.cc',
	'test/webfuse_provider/tests/core/timer/test_timepoint.cc',
	'test/webfuse_provider/tests/core/timer/test_timer.cc',
	'test/webfuse_provider/utils/tempdir.cc',
	'test/webfuse_provider/utils/file_utils.cc',
	'test/webfuse_provider/utils/timeout_watcher.cc',
	'test/webfuse_provider/utils/path.c',
	'test/webfuse_provider/utils/static_filesystem.c',
	'test/webfuse_provider/utils/ws_server.cc',
	'test/webfuse_provider/utils/ws_server2.cc',
	'test/webfuse_provider/utils/jansson_test_environment.cc',
	'test/webfuse_provider/mocks/fake_invokation_context.cc',
	'test/webfuse_provider/mocks/mock_request.cc',
	'test/webfuse_provider/mocks/mock_provider_client.cc',
	'test/webfuse_provider/mocks/mock_provider.cc',
	'test/webfuse_provider/mocks/mock_jsonrpc_proxy.cc',
	'test/webfuse_provider//tests/core/test_util.cc',
	'test/webfuse_provider/tests/core/test_container_of.cc',
	'test/webfuse_provider/tests/core/test_slist.cc',
	'test/webfuse_provider/tests/core/test_base64.cc',
	'test/webfuse_provider/tests/core/test_status.cc',
	'test/webfuse_provider/tests/core/test_message.cc',
	'test/webfuse_provider/tests/core/test_message_queue.cc',
	'test/webfuse_provider/tests/core/test_url.cc',
	'test/webfuse_provider/tests/provider/test_client_protocol.cc',
	'test/webfuse_provider/tests/provider/test_dirbuffer.cc',
	'test/webfuse_provider/tests/provider/operation/test_close.cc',
	'test/webfuse_provider/tests/provider/operation/test_getattr.cc',
	'test/webfuse_provider/tests/provider/operation/test_lookup.cc',
	'test/webfuse_provider/tests/provider/operation/test_open.cc',
	'test/webfuse_provider/tests/provider/operation/test_read.cc',
	'test/webfuse_provider/tests/provider/operation/test_readdir.cc',
	link_args: [
		'-Wl,--wrap=wf_timer_manager_create',
		'-Wl,--wrap=wf_timer_manager_dispose',
		'-Wl,--wrap=wf_timer_manager_check',
		'-Wl,--wrap=wf_timer_create',
		'-Wl,--wrap=wf_timer_dispose',
		'-Wl,--wrap=wf_timer_start',
		'-Wl,--wrap=wf_timer_cancel',
		'-Wl,--wrap=wf_impl_operation_context_get_proxy',
		'-Wl,--wrap=wf_jsonrpc_proxy_vinvoke',
		'-Wl,--wrap=wf_jsonrpc_proxy_vnotify',
	],
	include_directories: ['include', 'lib', 'test'],
	dependencies: [
		webfuse_provider_static_dep,
		libwebsockets_dep,
		jansson_dep,
		gtest_dep,
		gmock_main_dep,
		test_certs_dep
	])

test('alltests', alltests)

endif

# Examples

if not without_examples

executable('webfuse-provider',
    'examples/provider/main.c',
    dependencies: [webfuse_provider_dep])

executable('static-filesystem-provider',
	'examples/static_filesystem/static_filesystem.c',
	'examples/static_filesystem/path.c',
	'examples/static_filesystem/main.c',
    dependencies: [webfuse_provider_dep])


endif