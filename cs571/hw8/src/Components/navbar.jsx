import React from 'react';
import Navbar from 'react-bootstrap/Navbar';
import '../App.css';

function NavBar(props){
    return (
        <Navbar className="react-navbar" expand={"md"}>
            {props.searchBar}
            <Navbar.Toggle aria-controls="basic-navbar-nav" />
            <Navbar.Collapse id="basic-navbar-nav">
                    {props.children}
            </Navbar.Collapse>
        </Navbar>
      );
}
 
export default NavBar;