%linecount: determines the number of lines in a monte-carlo file.  
%the number of lines should be the top line of the file.

%returns [] if the file does not exist
%        [int] the number of lines to be read in if it does.

%input: string file

% silviana amethyst
% colorado state university
% mathematics
% 2013 
% silvianaamethyst@gmail.com
function linecount = getlinecount(file)

if ~ischar(file)
	display(sprintf('error: input file name was not a string'));
	linecount = [];
	return;
end
	
	
if isempty(dir(file))
	display(sprintf('error: requested file ''%s'' does not exist.',file));
	linecount = [];
	return;
end


runme = ['!wc -l ' file ' > wc.out'];
eval(runme);
wc_fid = fopen('wc.out');
linecount = fscanf(wc_fid,'%i')+1;
fclose(wc_fid);
!rm wc.out

linecount = linecount - is_last_line_empty(file);

% fid = fopen(file,'r');
% linecount = fscanf(fid,'%i',[1 1]);
% fclose(fid);

end



%some legacy code:


