! GNU General Public License v3.0
!
! This file is part of the GAHM model (https://github.com/adcirc/gahm).
! Copyright (c) 2023 ADCIRC Development Group.
!
! This program is free software: you can redistribute it and/or modify
! it under the terms of the GNU General Public License as published by
! the Free Software Foundation, version 3.
!
! This program is distributed in the hope that it will be useful, but
! WITHOUT ANY WARRANTY; without even the implied warranty of
! MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
! General Public License for more details.
!
! You should have received a copy of the GNU General Public License
! along with this program. If not, see <http://www.gnu.org/licenses/>.
!
! Author: Zach Cobell
! Contact: zcobell@thewaterinstitute.org
!
module test_criteria
    implicit none

    real(8), parameter :: tol = 1.0e-6

    contains

    logical function is_equal(a, b)
        implicit none
        real(8), intent(in) :: a, b
        is_equal = abs(a - b) <= tol
    end function is_equal

end module test_criteria

program TEST_VortexFortran
    implicit none
    !...Note that we're calling inside a subroutine.
    ! This is because OOP in fortran is a bit different
    ! than in other languages and the cleanup of the
    ! object is not done at the end of a main program,
    ! only when there is a return from another function/subroutine
    call test_001()
end program TEST_VortexFortran

subroutine test_001()
    use test_criteria, only: is_equal
    use gahm_module
    implicit none

    type(gahm_t)                       :: gahm
    type(date_t)                       :: start_date, current_date
    character(200)                     :: filename
    integer                            :: i, j
    integer(8)                         :: n_pts
    real(8),allocatable                :: x(:), y(:), u(:), v(:), p(:)
    logical                            :: exists
    real(8), allocatable               :: solution(:,:)
    real(8),parameter                  :: solution_vec(49*3) = (/&
             -.098232847d0,  -.569134195d0, 1012.696318027d0,&
             -.114892273d0,  -.641889644d0, 1012.657525639d0,&
             -.133488187d0,  -.719813591d0, 1012.615937386d0,&
             -.154160487d0,  -.803026278d0, 1012.571469352d0,&
             -.177053342d0,  -.891640762d0, 1012.524037430d0,&
             -.202314919d0,  -.985761975d0, 1012.473557494d0,&
             -.230097074d0, -1.085485817d0, 1012.419945564d0,&
             -.260555017d0, -1.190898299d0, 1012.363117962d0,&
             -.293846941d0, -1.302074713d0, 1012.302991460d0,&
             -.330133628d0, -1.419078869d0, 1012.239483436d0,&
             -.369578027d0, -1.541962376d0, 1012.172512013d0,&
             -.412344803d0, -1.670763988d0, 1012.101996208d0,&
             -.458599875d0, -1.805509014d0, 1012.027856068d0,&
             -.475039450d0, -1.831276100d0, 1012.015618933d0,&
             -.491906444d0, -1.857097224d0, 1012.003417641d0,&
             -.509206506d0, -1.882958393d0, 1011.991257086d0,&
             -.526945068d0, -1.908844986d0, 1011.979142309d0,&
             -.545127320d0, -1.934741741d0, 1011.967078503d0,&
             -.563758189d0, -1.960632742d0, 1011.955071015d0,&
             -.582842312d0, -1.986501412d0, 1011.943125346d0,&
             -.602384016d0, -2.012330504d0, 1011.931247155d0,&
             -.622387285d0, -2.038102087d0, 1011.919442263d0,&
             -.642855736d0, -2.063797541d0, 1011.907716647d0,&
             -.663792591d0, -2.089397551d0, 1011.896076451d0,&
             -.685200643d0, -2.114882097d0, 1011.884527980d0,&
             -.692316993d0, -2.107691938d0, 1011.902196947d0,&
             -.699676404d0, -2.101039107d0, 1011.919336554d0,&
             -.707285467d0, -2.094916818d0, 1011.935963413d0,&
             -.715150961d0, -2.089318473d0, 1011.952093585d0,&
             -.723314332d0, -2.084336991d0, 1011.967692041d0,&
             -.731697209d0, -2.079718933d0, 1011.982899939d0,&
             -.740363228d0, -2.075621932d0, 1011.997647738d0,&
             -.749320376d0, -2.072040544d0, 1012.011949554d0,&
             -.758576886d0, -2.068969501d0, 1012.025819037d0,&
             -.768141246d0, -2.066403693d0, 1012.039269384d0,&
             -.778022201d0, -2.064338163d0, 1012.052313356d0,&
             -.788228763d0, -2.062768091d0, 1012.064963296d0,&
             -.830107393d0, -2.154209547d0, 1012.016183106d0,&
             -.874102846d0, -2.249268497d0, 1011.964999249d0,&
             -.920324335d0, -2.348084403d0, 1011.911283679d0,&
             -.968872274d0, -2.450764814d0, 1011.854918268d0,&
            -1.019850410d0, -2.557416734d0, 1011.795779848d0,&
            -1.073365832d0, -2.668146261d0, 1011.733740031d0,&
            -1.129528966d0, -2.783058198d0, 1011.668665029d0,&
            -1.188453568d0, -2.902255654d0, 1011.600415470d0,&
            -1.250256710d0, -3.025839605d0, 1011.528846214d0,&
            -1.315058743d0, -3.153908448d0, 1011.453806161d0,&
            -1.382983270d0, -3.286557524d0, 1011.375138064d0,&
            -1.454157093d0, -3.423878633d0, 1011.292678338d0/)

    !...Reshape into n x 3 array
    solution = transpose(reshape(solution_vec, (/3,49/)))

    !...Initialization of GAHM begins

    !...Allocate memory for 1 point where we will validate the solution
    allocate(x(1))
    allocate(y(1))
    allocate(u(1))
    allocate(v(1))
    allocate(p(1))

    !...Known good solution
    x(1) = -90.0
    y(1) = 29.0

    filename = "../tests/test_files/bal122005.dat"

    inquire(file=filename, exist=exists)
    if (.not.exists) then
        write(*,'(A)') "[ERROR]: File does not exist"
        call exit(1)
    end if

    n_pts = size(x)

    call gahm%initialize(filename, n_pts, x, y)
    call start_date%set(2005,8,27)
    !...Initialization of GAHM ends

    !...GAHM Time stepping loop
    j = 0
    do i = 0, 86400, 1800
        current_date = start_date%add(int(i, 8))
        call gahm%get(current_date, n_pts, u, v, p)

        j = j + 1

        !...Check the solution
        if (.not.is_equal(u(1), solution(j,1))) then
            write(*,'(A)') "[ERROR]: U values are not equal"
            write(*,'(A,F0.6)') "[ERROR]: u(1) = ", u(1)
            write(*,'(A,I0,A,F0.6)') "[ERROR]: solution(",j,",1) = ", solution(j,1)
            call exit(1)
        end if

        if (.not.is_equal(v(1), solution(j,2))) then
            write(*,'(A)') "[ERROR]: V values are not equal"
            write(*,'(A,F0.6)') "[ERROR]: v(1) = ", v(1)
            write(*,'(A,I0,A,F0.6)') "[ERROR]: solution(",j,",2) = ", solution(j,2)
            call exit(1)
        end if

        if (.not.is_equal(p(1), solution(j,3))) then
            write(*,'(A)') "[ERROR]: P values are not equal"
            write(*,'(A, F0.6)') "[ERROR]: p(1) = ", p(1)
            write(*,'(A,I0,A,F0.6)') "[ERROR]: solution(",j,",3) = ", solution(j,3)
            call exit(1)
        end if

        !write(*,'(F0.9,A,F0.9,A,F0.9,A)') u(1), ", ", v(1), ", ", p(1), ", "

    end do

end subroutine test_001