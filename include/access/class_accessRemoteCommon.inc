<?php




/* small php classe dedicated to test value */
class accessRemoteValidation{

    static function runTest($rule,$value,$range=array()){
        if ( !empty($range)){
            return(self::$rule($value,$range));  
        }else{
            return(self::$rule($value));
        }
        
    }
    
    static private function is_bool($val, $range=array()){
        return(filter_var($val,FILTER_VALIDATE_BOOLEAN));
    }
    
    static private function is_integer($val, $range=array()){
        if ( ! empty($range)){
            //return(filter_var($val,FILTER_VALIDATE_INT,$range[0],$range[1]));
            return(in_array($val,range($range[0],$range[1])));
        }else{
            return(filter_var($val,FILTER_VALIDATE_INT));
        }
    }

    static private function is_string($val, $range=array()){
        return(is_string($val));
    }
    
    
    static private function is_url($val, $range=array()){
        return(filter_var($val,FILTER_VALIDATE_URL));
    }

    static private function is_select($val,$range){
        return(in_array($val,$range));
    }

}


class accessRemoteCommon {

    private $cnx=array();
    private $uri="";
    private $headers = array('Accept' => 'application/json', 'Content-Type' => 'application/json');
    private $param = array();

    private $currentMapping=array();


    static function list_accessRemoteMethod(){


        global $class_mapping;
        $available = [];
        foreach ($class_mapping as $class => $path) {
          if (preg_match('/^accessRemote(.+)/', $class, $m)) {
            //$available[$m[1]] = $class::getName();
            $available[]=$class;
          }
        }

        return($available);
    }


    /* method user for testing value fore each options per protocol */
    public function getValidation($attr){
        foreach(self::$mappingFD as $protocol){
            foreach($protocol as $k => $v){
                if ( $k == $attr){
                    return($v[$k]['validation']);
                }
            }
        }

    }



    public function __construct($cnx) {
        $this->cnx = $cnx;
    }

    public function checkStatus(){
        $status="All's is OK, should remplaced by a real test ;)";
	    return($result);
	}

   

}
