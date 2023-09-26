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
    real(8),parameter, dimension(48*3) :: solution_vec = (/&
            .195298d0,  .770393d0, 1012.501991d0,&
            .173304d0, -.189623d0, 1012.652625d0,&
            .178024d0, -.196322d0, 1012.635749d0,&
            .187245d0, -.206677d0, 1012.614520d0,&
            .196831d0, -.217412d0, 1012.592489d0,&
            .206694d0, -.228460d0, 1012.569724d0,&
            .216839d0, -.239824d0, 1012.546214d0,&
            .227270d0, -.251511d0, 1012.521945d0,&
            .237996d0, -.263527d0, 1012.496905d0,&
            .249022d0, -.275879d0, 1012.471079d0,&
            .260355d0, -.288573d0, 1012.444454d0,&
            .272004d0, -.301616d0, 1012.417014d0,&
            .283968d0, -.315009d0, 1012.388759d0,&
            .283431d0, -.315154d0, 1012.387664d0,&
            .281750d0, -.314367d0, 1012.387720d0,&
            .280016d0, -.313521d0, 1012.387903d0,&
            .278273d0, -.312649d0, 1012.388173d0,&
            .276522d0, -.311754d0, 1012.388525d0,&
            .274764d0, -.310835d0, 1012.388959d0,&
            .272998d0, -.309894d0, 1012.389474d0,&
            .271226d0, -.308930d0, 1012.390069d0,&
            .269448d0, -.307944d0, 1012.390741d0,&
            .267663d0, -.306935d0, 1012.391491d0,&
            .265873d0, -.305905d0, 1012.392317d0,&
            .264075d0, -.304852d0, 1012.393224d0,&
            .258922d0, -.300131d0, 1012.406870d0,&
            .253322d0, -.294990d0, 1012.420902d0,&
            .247751d0, -.289832d0, 1012.434829d0,&
            .242235d0, -.284679d0, 1012.448615d0,&
            .236780d0, -.279540d0, 1012.462245d0,&
            .231391d0, -.274422d0, 1012.475706d0,&
            .226073d0, -.269333d0, 1012.488985d0,&
            .220831d0, -.264277d0, 1012.502072d0,&
            .215668d0, -.259261d0, 1012.514960d0,&
            .210586d0, -.254288d0, 1012.527640d0,&
            .205587d0, -.249362d0, 1012.540107d0,&
            .200679d0, -.244493d0, 1012.552342d0,&
            .203485d0, -.248997d0, 1012.540164d0,&
            .207094d0, -.254291d0, 1012.526841d0,&
            .210821d0, -.259750d0, 1012.513085d0,&
            .214649d0, -.265362d0, 1012.498905d0,&
            .218579d0, -.271129d0, 1012.484287d0,&
            .222616d0, -.277057d0, 1012.469216d0,&
            .226762d0, -.283151d0, 1012.453676d0,&
            .231022d0, -.289418d0, 1012.437650d0,&
            .235399d0, -.295863d0, 1012.421122d0,&
            .239898d0, -.302492d0, 1012.404074d0,&
            .244522d0, -.309311d0, 1012.386488d0 &
        /)

    !...Reshape into n x 3 array
    solution = transpose(reshape(solution_vec, (/3,48/)))

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
    do i = 1, 86400, 1800
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

    end do
end subroutine test_001