classdef ArdStim < handle
    % This class for connecting to, reading from, and closing the stimBox
    %
    % Example usage:
    % AS = ArdStim;
    % 
    %
    % Date: 20240105
    % Author: Marshall Trout
    
    properties
        ARD; COMStr; COMPoint; Status; Ready; LoopCnt;
    end
    
    methods
        function obj = ArdStim(varargin)
            obj.Ready = false;
            obj.LoopCnt = 1;
%             obj.Status.ElapsedTime = nan;
%             obj.Status.CurrTime = clock;
%             obj.Status.LastTime = clock;
            obj.Status.Count = 1;
            obj.Status.ard_time = 0;
            init(obj);
        end
        function init(obj,varargin)
            devs = getSerialID;
            if ~isempty(devs)
                COMPort = cell2mat(devs(~cellfun(@isempty,regexp(devs(:,1),'Arduino Uno')),2));
                if isempty(COMPort)
                    COMPort = cell2mat(devs(~cellfun(@isempty,regexp(devs(:,1),'Silicon Labs CP210x USB to UART Bridge')),2));
                elseif isempty(COMPort)
                    COMPort = cell2mat(devs(~cellfun(@isempty,regexp(devs(:,1),'USB-SERIAL CH340')),2));
                end
            end
            if ~isempty(COMPort)
                obj.COMStr = sprintf('COM%0.0f',COMPort(end));
            end
            delete(instrfind('port',obj.COMStr));
            
            obj.COMPoint = openPort(obj.COMStr,115200);       
            disp("StimBox Connecting...");
            obj.clearSerialBuff;
            tic;
            while(obj.Status.ard_time == 0)
                if toc > 1
                    disp("Ping...");
                    obj.stim([0,0,0,0,0,0,0,0]);
                    tic;
                end
            end
            obj.stim([0,0,0,0,0,0,0,0]); pause(0.15);
            obj.stim([0,0,0,0,0,0,0,0]); pause(0.15);
            obj.clearSerialBuff;
            disp("StimBox Connected!");
            obj.Ready = true;
        end
        
        function close(obj,varargin)
            try
                prev_ard_time = obj.Status.ard_time;
                obj.stim([0,0,0,0,0,0,0,0]);
                obj.clearSerialBuff;
                if obj.Status.ard_time > prev_ard_time
                    closePort(obj.COMPoint);
                    disp("StimBox Disconnected...");
                else
                    disp("StimBox Failed to Disconnected...");
                    disp("Unplug USB...");
                end
                obj.Ready = false;
            catch
                disp("StimBox Failed to Disconnected...");
                disp("Unplug USB...");
                obj.Ready = false;
            end
        end
        
        function serialRead(obj,varargin)
            try
                dataIn = [];
                [tempDataIn , obj.Status.leftOverBytesInBuffer] = readPort(obj.COMPoint, 12);
                dataIn = [dataIn tempDataIn];
                dataIn = double(dataIn);
                obj.Status.returned_values = dataIn(5:end);
                obj.Status.ard_time = dataIn(1)*2^0+dataIn(2)*2^8+...
                    dataIn(3)*2^16+dataIn(4)*2^24;
                
%                 if obj.Status.leftOverBytesInBuffer > 0
%                     obj.clearSerialBuff;
%                 end

                           
            catch
                disp('StimBox serial read error...')
            end
        end
        
        
        
        function stim(obj,varargin)
            stim = varargin{1};
            try
                obj.Status.potval1 = floor(stim(1)*16.25);
                
                obj.Status.pulsewidth1 = floor(stim(2)*.2);
                
                obj.Status.potval2 = floor(stim(3)*16.25);

                obj.Status.pulsewidth2 = floor(stim(4)*.2);
                
                obj.Status.potval3 = floor(stim(5)*16.25);
                
                obj.Status.pulsewidth3 = floor(stim(6)*.2);

                obj.Status.freq = floor(stim(7));
              
                % write data
                obj.Status.dataOut = [obj.Status.potval1,...
                    obj.Status.pulsewidth1,...
                    obj.Status.potval2,...
                    obj.Status.pulsewidth2,...
                    obj.Status.potval3,....
                    obj.Status.pulsewidth3,...
                    obj.Status.freq,...
                    stim(8)];
                obj.Status.dataOut = max(obj.Status.dataOut,0);
                obj.Status.dataOut = min(obj.Status.dataOut,255);
                obj.Status.dataOut = uint8(obj.Status.dataOut);
                writePort(obj.COMPoint, obj.Status.dataOut);
                pause(0.001);
                
              
                % get Time
                obj.serialRead;
                           
            catch
                disp('StimBox stim error...')
                obj.Ready = false;
            end
        end
        
        function clearSerialBuff(obj, varargin)
            [dataIn , obj.Status.leftOverBytesInBuffer] = readPort(obj.COMPoint, 1);
            while obj.Status.leftOverBytesInBuffer > 0
                [dataIn , obj.Status.leftOverBytesInBuffer] = readPort(obj.COMPoint, obj.Status.leftOverBytesInBuffer);
            end
        end
    end    
end %class