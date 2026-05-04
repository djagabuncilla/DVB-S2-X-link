function [txWaveform, bitStream, payloadStream] = generate_dvbs2_frames(numFrames)

    packetsPerFrame = 4;
    pktBytes = 188;
    payloadBytes = pktBytes - 1; % без sync
    pktBits = pktBytes * 8;

    totalPackets = numFrames * packetsPerFrame;

    bitStream = false(totalPackets * pktBits,1);
    payloadStream = false(totalPackets * payloadBytes * 8,1);

    idxBits = 1;
    idxPayload = 1;

    for k = 1:totalPackets

        pkt = zeros(pktBytes,1,'uint8');
        pkt(1) = hex2dec('47'); % sync
        pkt(2:end) = randi([0 255],pktBytes-1,1);

        % --- сохраняем payload отдельно (для сравнения) ---
        payload = pkt(2:end);
        payloadBits = de2bi(payload,8,'left-msb')';
        payloadBits = logical(payloadBits(:));

        payloadStream(idxPayload:idxPayload+length(payloadBits)-1) = payloadBits;
        idxPayload = idxPayload + length(payloadBits);

        % --- полный пакет ---
        bits = de2bi(pkt,8,'left-msb')';
        bits = logical(bits(:));

        bitStream(idxBits:idxBits+pktBits-1) = bits;
        idxBits = idxBits + pktBits;
    end

    % --- DVB-S2 генератор ---
    gen = dvbs2WaveformGenerator( ...
        'StreamFormat','TS', ... 
        'NumInputStreams',1, ...
        'FECFrame','short', ...
        'MODCOD',3, ...
        'HasPilots',true, ...
        'DFL', 6232, ...
        'SamplesPerSymbol',4);

    txWaveform = gen(bitStream);
end