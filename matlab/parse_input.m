%parse_input: parses a paramatopy input file and generates a matlab structure with
%relevant data.
% input: filename, a char array indicating the name of the paramotopy input
% file to parse.
% returns: info, a struct with the file info in it.
% 
% paraminfo: first column: left endpoint, real
%           second column: left endpoint, imag
%           third column : right endpoint, real
%           fourth       : right endpoint, imag
%           fifth        : number mesh points
%
%silviana amethyst
%Mathematics
%Colorado State University
%2011-2013
%silvianaamethyst@gmail.com

function [info] = parse_input(filename)



%%%%%%%%%%%%%%%%%%%%%%%%
%%%
%%%         parse the input file
%%%
%%%%%%%%%%%%%%%%%%%%%%%%
fid = fopen(filename,'r');
inputfileparameters = fscanf(fid,'%i %i %i %i\n'); %neqns nvargroups nparameters
info.filename = filename;
info.numeqn = inputfileparameters(1);
info.numvargroup = inputfileparameters(2);
info.numparam = inputfileparameters(3);
info.numconst = inputfileparameters(4);

% read in the equations
info.eqn = cell(1,info.numeqn);
for ii = 1:info.numeqn
    info.eqn{ii} = fgetl(fid);
end


% read in the variable groups
variable_groups = cell(1,info.numvargroup);
for ii = 1:info.numvargroup
    variable_groups{ii} = fgetl(fid);  %get entire line
end


% parse variable groups into variables
info.vars = cell(1,0);  % make an empty 1d cell array
var_counter = 0;
for ii = 1:info.numvargroup
    vars = split(variable_groups{ii},','); % split on commas
    
    % strip whitespace and pack into the cell array
    for jj = 1:length(vars)
        v = vars(jj);
        var_counter = var_counter+1;
        info.vars(var_counter) = strtrim(v);
    end

end


%%%%%%%%constants
if info.numconst>0
	info.constnames = cell(1,info.numconst);
	info.constvalues = cell(1,info.numconst);
	tmp=fgetl(fid);  %waste the line corresponding to the constant declaration
for ii = 1:info.numconst
	tmp = fgetl(fid);
	tmp = deblank(tmp);
	info.constnames{ii} = tmp(1:strfind(tmp,'=')-1);
	info.constvalues{ii} = tmp(strfind(tmp,'=')+1:end-1);
end
end


%is run userdefined?
tmp = fgetl(fid);
ind = isspace(tmp);
if all(ind==0)
	info.userdefined = str2num(tmp);
else
	while isspace(tmp(1))
		tmp = tmp(2:end);
	end
	ind = find(isspace(tmp));
	info.userdefined = str2num(tmp(1:ind-1));
end
% info.userdefined = fscanf(fid,'%i\n',[1,1]);  

% parse in the parameters
info.paramnames = cell(info.numparam,1);
if (info.userdefined==0)
	info.paramvalues = zeros(info.numparam,5);  %preallocate
        for ii = 1:info.numparam
			tmp = fscanf(fid,'%s',[1,1]);
            info.paramnames{ii} = tmp;  %get name
            for jj = 1:5
				tmp = fscanf(fid,'%f',[1,1]);
                info.paramvalues(ii,jj) = tmp;  %get mesh info
            end


        end
        % paraminfo: first column: left endpoint, real
        %           second column: left endpoint, imag
        %           third column : right endpoint, real
        %           fourth       : right endpoint, imag
        %           fifth        : number mesh points
elseif (info.userdefined==1)
    info.userparameterfile = fscanf(fid,'%s\n',[1,1]); %get file name
	for ii = 1:inputfileparameters(3)
		templine = fgetl(fid);
			while isspace(templine(1)) % now tolerates extra crap being on the line!!!  yay!
				templine = templine(2:end);
			end
			spaces = find(isspace(templine));
			
			if isempty(spaces)
				info.paramnames{ii} = templine; %get name
			else
				info.paramnames{ii} = templine(1:spaces(1)-1); %get name
			end
	end
	
	%seems like i am missing a few pieces here...
	
else
	display('error reading the userdefined bool')
	
end%re: if userdefined

%finally, get the rest of the lines as supplementary lines
info.supplementary = cell(1,1);
counter = 0;
while (~feof(fid))
	counter = counter+1;
	info.supplementary{counter} = fgetl(fid);
end

 

fclose(fid);

info.numvar = length(info.vars);


end