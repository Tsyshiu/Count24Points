function Enter-Downloads { cd F:\Desktop\Downloads }
Set-Alias -Name code -Value "E:\Program Files\Microsoft VS Code\bin\code.cmd"

<#
.Synopsis
   pow函数
#>
function pow {
    Param
    (
        [parameter(Mandatory=$true,Position=0)]
        [double[]]
        $a
    )
    $powRes=1
    for ($i = 1; $i -le $a[1]; $i++) {
        $powRes = $powRes * $a[0]
    }
    return $powRes
}

<#
.Synopsis
   进入vs编译器
#>
function Enter-Compiler
{
    cmd /k "C:\Users\acer\Documents\WindowsPowerShell\zxCompiling.bat"
}

<#
.Synopsis
   删除编译器产生的文件
.DESCRIPTION
   mode:"All", "LinkOnly(Default)", "LinkAndPDB" 
#>
function Remove-CompiledItem 
{ 
    [CmdletBinding()]
    Param
    (
        [ValidateSet("All", "LinkOnly", "LinkAndPDB")]
        [String]
        $Mode="LinkOnly",

        [parameter(Position=0)]
        [String[]]
        $Path="*"
    )
    if ($Mode -eq "LinkOnly")
    {
        Remove-Item -Path $Path -Include *.obj,*.ilk
        Write-Host *.obj,*.ilk in $Path have been cleared
    }
    elseif ($Mode -eq "All")
    {    
        Remove-Item -Path $Path -Include *.obj,*.ilk,*.pdb,*.exe
        Write-Host *.obj,*.ilk,*.pdb, in $Path have been cleared
    }
    else
    {
        Remove-Item -Path $Path -Include *.obj,*.ilk,*.pdb
        Write-Host all in $Path have been cleared
    }
}
Set-Alias clrm -Value Remove-CompiledItem