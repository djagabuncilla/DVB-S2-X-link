function CMakeBuild()
% CMakeBuild - автоматическая сборка C++ библиотеки dvbs2.dll
% Вызывать из startup.mlx или командной строки MATLAB

    % Определяем пути относительно текущего рабочего каталога
    prj_root = pwd;
    cpp_path = fullfile(prj_root, 'cpp');
    build_path = fullfile(cpp_path, 'build');
    
    % Создаём папку build, если нет
    if ~exist(build_path, 'dir')
        mkdir(build_path);
    end
    
    % CMake configure: -S исходники, -B сборка
    fprintf('🔧 Configuring CMake...\n');
    cmake_configure = sprintf('cmake -S "%s" -B "%s" -G "MinGW Makefiles"', ...
        cpp_path, build_path);
    [status_cfg, output_cfg] = system(cmake_configure);
    if status_cfg ~= 0
        error('❌ CMake configure failed:\n%s', output_cfg);
    end
    
    % CMake build
    fprintf('🔨 Building library...\n');
    cmake_build = sprintf('cmake --build "%s"', build_path);
    [status_build, output_build] = system(cmake_build);
    if status_build ~= 0
        error('❌ CMake build failed:\n%s', output_build);
    end
    
    fprintf('✅ Library built: %s\n', fullfile(build_path, 'dvbs2.dll'));
end