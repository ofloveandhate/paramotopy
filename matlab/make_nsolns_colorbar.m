% make_nsolns_colorbar(maxiii,fontsize)
%
% makes a colorbar for a data set, and sets a colormap to correspond.
% the data set must be integer, and be positive (0 ok).
%
% input: integer maxiii, the upper limit for the colorbar.  must be an
%            integer.
%        integer fontsize, an integer for the size of the labels, etc.
%
% output: none
%
% default fontsize is 16. 
%
% silviana amethyst
% colorado state university
% mathematics
% 2013
% silvianaamethyst@gmail.com


function make_nsolns_colorbar(maxiii,fontsize)

max_num_bins = 7;

if nargin<2
	fontsize = 16;
end

if maxiii ==0
	maxiii = 1;
end

J = jet(maxiii+1);
colormap(J);
set(gca,'CLim',[0 1]);

cbar = colorbar;
f = factor(maxiii);


if maxiii<=max_num_bins
	labels = 0:maxiii;
elseif min(f)>max_num_bins
	num_bins = max_num_bins;
	labels = 0:(maxiii)/num_bins:maxiii;
	labels = ceil(labels);
	if labels(end)>((max_num_bins-2)/max_num_bins*maxiii)
		labels = [labels(1:end-1) maxiii];
	else
		labels = [labels(1:end) maxiii];
		
	end

else
	
	small_factors = f(f<max_num_bins);
	
	if prod(small_factors)>max_num_bins
		if max(small_factors)>4
			num_bins = max(small_factors);
		else
			num_bins = 1;
			counter = 1;
			while (num_bins*f(counter)<max_num_bins)
				num_bins= num_bins*f(counter);
				counter = counter+1;
			end
		end
	else
		num_bins = prod(small_factors);
	end
	

	labels = 0:(maxiii)/num_bins:maxiii; 
end
% (2*(maxiii+1)-1)/(2*(maxiii+1))
% 
% labels
% labels/maxiii+1
placements = labels/(maxiii)*((2*(maxiii+1)-1)/(2*(maxiii+1)) - 1/(2*(maxiii+1))) + 1/(2*(maxiii+1)); % +
%rescale labels
% numbins = min(,maxiii+1);
% placements = linspace(1/(2*(maxiii+1)),(2*(maxiii+1)-1)/(2*(maxiii+1)), num_bins)  % puts the labels in the center of the colorbar sections

% labels = 

set(cbar,'Ytick',placements,'YTicklabel',labels);
set(gca,'FontSize',fontsize-2);
set(get(cbar,'ylabel'),'String', '# Solns','FontSize',fontsize-2);
		
figure(gcf)
end