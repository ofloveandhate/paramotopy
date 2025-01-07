% is_last_line_empty: determines whether the last character of a file
%     is the newline character.
% input: char filename.
% output: 1 if last character is newline, 0 if not.
%
% does NOT return the number of new lines, just whether it has one or not.
%

%this file created with help from the internets.
%
% silviana amethyst
% Mathematics
% Colorado State University
% 2011-2013
% silvianaamethyst@gmail.com

function value = is_last_line_empty(filename)


fid = fopen(filename,'r');     % Open the file

fseek(fid,-1,'eof');        % Seek to the file end, less one character
lastchar = fread(fid,1,'*char');  % Read one character
if (~strcmp(lastchar,char(10))) % check if it is newlines
	value = 0;
else
	value = 1;
end


end