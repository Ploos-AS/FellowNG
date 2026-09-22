/* FellowNG M8.3 ARexx m68k baseline probe v1 */
numeric digits 9
failures = 0

say 'FELLOWNG_AREXX_PROBE_V1 BEGIN'

call check 'assignment', answer = 42
call check 'expression', (6 * 7) = 42
call check 'string_concat', ('Fellow' || 'NG') = 'FellowNG'
call check 'string_length', length('FellowNG') = 8
call check 'string_upper', translate('FellowNG') = 'FELLOWNG'
call check 'arithmetic_add', (20 + 22) = 42
call check 'arithmetic_div', (84 / 2) = 42

flag = 0
if 42 > 7 then flag = 1
call check 'conditional', flag = 1

sum = 0
do i = 1 to 6
  sum = sum + i
end
call check 'loop', sum = 21

call addtwo 20, 22
call check 'procedure_result', result = 42

call identity 'FellowNG'
call check 'return_result', result = 'FellowNG'

if failures = 0 then do
  say 'FELLOWNG_AREXX_PROBE_V1 PASS'
  exit 0
end

say 'FELLOWNG_AREXX_PROBE_V1 FAIL' failures
exit 10

check: procedure expose failures
  parse arg id, ok
  if ok then
    say 'TEST' id 'PASS'
  else do
    say 'TEST' id 'FAIL assertion'
    failures = failures + 1
  end
return

addtwo: procedure
  parse arg a, b
return a + b

identity: procedure
  parse arg value
return value
