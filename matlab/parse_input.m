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


info.eqn = cell(1,info.numeqn);
for ii = 1:info.numeqn
    info.eqn{ii} = fgetl(fid);
end

varcrap = cell(1,info.numvargroup);
for ii = 1:info.numvargroup
    varcrap{ii} = fgetl(fid);  %get entire line
end
info.vars = cell(1,info.numeqn);
varcounter = 1;
for ii = 1:info.numvargroup
    commas = strfind(varcrap{ii},',');%finds commas, which we use to separate the variables in a group
    if isempty(commas)
        info.vars{varcounter} = varcrap{ii}; %only one variable on line, so grab it
        varcounter = varcounter + 1;
    else
        tmp = varcrap{ii};                 %because working with cells sometimes is awkward
        tmp = [',' tmp(~isspace(tmp)) ','];%despace
        commas = strfind(tmp,',');         %find commas because of omission of spaces
        for jj = 1:length(commas)-1        % for each found comma
            info.vars{varcounter} = tmp(commas(jj)+1:commas(jj+1)-1); %grab the variable name
            varcounter = varcounter + 1;
            
        end
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
	info.constvalues{ii} = str2double(tmp(strfind(tmp,'=')+1:end-1));
end
end


tmp = fgetl(fid);
ind = isspace(tmp);
if all(ind==0)
	info.userdefined = str2num(tmp);
else
	while isspace(tmp(1));
		tmp = tmp(2:end);
	end
	ind = find(isspace(tmp));
	info.userdefined = str2num(tmp(1:ind-1));
end
% info.userdefined = fscanf(fid,'%i\n',[1,1]);  %is run userdefined?

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