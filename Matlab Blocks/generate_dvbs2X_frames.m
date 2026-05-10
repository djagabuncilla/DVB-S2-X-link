function [txWaveform, bitStream, payloadStream] = generate_dvbs2X_frames(numFrames)
    % Генерация кадров DVB-S2X для VL-SNR Set 1, модуляция QPSK 2/9
    % ETSI EN 302 307-2, PLSDecimalCode = 129

    packetsPerFrame = 4;              % TS-пакетов на физический кадр (настраивается)
    pktBytes = 188;
    pktBits = pktBytes * 8;           % 1504 бита
    payloadBytes = pktBytes - 1;      % 187 байт payload (без sync)
    payloadBitsLen = payloadBytes * 8;% 1496 бит

    totalPackets = numFrames * packetsPerFrame;

    % Предвыделение памяти
    bitStream = false(totalPackets * pktBits, 1);
    payloadStream = false(totalPackets * payloadBitsLen, 1);

    idxBits = 1;
    idxPayload = 1;

    for k = 1:totalPackets
        % Формирование TS-пакета (188 байт)
        pkt = zeros(pktBytes, 1, 'uint8');
        pkt(1) = hex2dec('47');  % Sync byte (фиксирован для TS)
        pkt(2:end) = randi([0 255], pktBytes-1, 1);

        % Сохранение payload отдельно (для верификации)
        payload = pkt(2:end);
        payloadBitsVec = de2bi(payload, 8, 'left-msb')';
        payloadBitsVec = logical(payloadBitsVec(:));
        payloadStream(idxPayload:idxPayload+length(payloadBitsVec)-1) = payloadBitsVec;
        idxPayload = idxPayload + length(payloadBitsVec);

        % Преобразование всего пакета в биты
        bits = de2bi(pkt, 8, 'left-msb')';
        bits = logical(bits(:));
        bitStream(idxBits:idxBits+pktBits-1) = bits;
        idxBits = idxBits + pktBits;
    end

    % === DVB-S2X генератор: VL-SNR Set 1, QPSK 2/9 ===
    gen = dvbs2xWaveformGenerator( ...
        'StreamFormat', 'TS', ...                % Transport Stream
        'NumInputStreams', 1, ...
        'PLSDecimalCode', 129, ...               % КЛЮЧЕВОЕ: 129 = VL-SNR Set 1
        'CanonicalMODCODName', 'QPSK 2/9', ...   % Модуляция и код (только для VL-SNR)
        'DFL', 6232, ...                         % Data Field Length в битах (см. примечание ниже)
        'SamplesPerSymbol', 4, ...
        'PLScramblingIndex', 0, ...
        'RolloffFactor', 0.35);

    % Проверка совместимости длины входных данных
    % Для TS-формата: длина bitStream должна быть кратна:
    %   gen.MinNumPackets * 1504  (1504 = UPL для TS)
    % Если нужно — обрежьте или дополните bitStream до нужной длины:
    %   requiredLen = gen.MinNumPackets * numFrames * 1504;
    %   bitStream = bitStream(1:requiredLen);

    txWaveform = gen(bitStream);
end