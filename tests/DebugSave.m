function succ = DebugSave(m, key)
i=0;
while true
    fname = sprintf('/var/tmp/%s-%d-mat.csv', key, i);
    if exist(fname, 'file')
        i = i +1;
    else
        break;
    end
end
fprintf('Saving %s %s\n', key, fname);
dlmwrite(fname, m, 'delimiter', ',', 'precision', 9);
end