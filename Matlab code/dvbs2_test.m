function dvbs2_test()
% dvbs2_test - сборка и запуск теста dvbs2

    prj_root = pwd;

    test_path  = fullfile(prj_root, 'cpp', 'test');
    build_path = fullfile(test_path, 'build');

    if ~exist(build_path, 'dir')
        mkdir(build_path);
    end

    addpath(build_path);

    % --- CMake configure ---
    fprintf('🔧 Configuring test with CMake...\n');

    cmake_configure = sprintf('cmake -S "%s" -B "%s" -G "MinGW Makefiles"', ...
        test_path, build_path);

    [status_cfg, output_cfg] = system(cmake_configure);
    if status_cfg ~= 0
        error('CMake configure failed:\n%s', output_cfg);
    end

    % --- Build ---
    fprintf('🔨 Building test...\n');

    cmake_build = sprintf('cmake --build "%s"', build_path);

    [status_build, output_build] = system(cmake_build);
    if status_build ~= 0
        error('CMake build failed:\n%s', output_build);
    end

    % --- Run ---
    exe_path = fullfile(build_path, 'dvbs2_test.exe');

    if ~exist(exe_path, 'file')
        error('Test executable not found: %s', exe_path);
    end

    system(exe_path);
end