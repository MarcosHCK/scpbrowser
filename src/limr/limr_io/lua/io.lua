local io = ...

-------------------------------------------------------------------------------

function io.close (file)
  return (file or io.output()):close()
end

function io.flush ()
  return io.output ():flush ()
end

function io.lines (filename, ...)
  if filename then
    local file, reason = io.open (filename)
    if not file then
      error (reason, 2)
    end
    local args = table.pack (...)
    return function()
      local result = table.pack (file:read (table.unpack (args, 1, args.n)))
      if not result[1] then
        if result[2] then
          error(result[2], 2)
        else -- eof
          file:close ()
          return nil
        end
      end
      return table.unpack (result, 1, result.n)
    end
  else
    return io.input ():lines ()
  end
end

function io.input (file)
  if not file then
    return io.stdin
  else
    io.stdin = io.open (file, 'r')
  end
end

function io.output (file)
  if not file then
    return io.stdout
  else
    io.stdout = io.open (file, 'w')
  end
end

function io.read(...)
  return io.input ():read (...)
end

function io.write (...)
  return io.output ():write (...)
end

-------------------------------------------------------------------------------

return io
