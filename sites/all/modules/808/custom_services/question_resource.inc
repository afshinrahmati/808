<?php

function question_resource_index($hash , $domain , $page , $version){
    if(!(hash_true($hash , "question"))) return "failed to access";
    if($version == 0){
        return list_of_questions_version_one($domain , $page);
    }
    return "This version not supported";
}
function question_resource_retrieve($hash , $nid , $version){
    if(!(hash_true($hash , "question"))) return "failed to access";
    if($version == 0){
        return retrieve_a_question_version_one($nid);
    }
    return "This version not supported";
}
function question_resource_create($hash , $question , $version){
    if(!(hash_true($hash , "question"))) return "failed to access";
    if($version == 0){
        return create_a_question_version_one($question);
    }
    return "This version not supported";
}
function question_resource_edit($hash , $nid , $question , $version){
    if(!(hash_true($hash , "question"))) return "failed to access";
    if($version == 0){
        return edit_a_question_version_one($nid , $question);
    }
    return "This version not supported";
}
function question_resource_follow($hash , $nid , $data , $version){
    if(!(hash_true($hash , "question"))) return "failed to access";
    if($version == 0){
        return follow_a_question_version_one($nid , $data);
    }
    return "This version not supported";
}
function question_resource_vote($hash , $nid , $data , $version){
    if(!(hash_true($hash , "question"))) return "failed to access";
    if($version == 0){
        return vote_a_question_version_one($nid , $data);
    }
    return "This version not supported";
}

function list_of_questions_version_one($domain , $page){
    switch ($domain){
        case "all_questions" :
            $query = all_questions();
            break;
        case "my_questions" :
            $query = my_questions();
            break;
        case "following_questions" :
            $query = following_questions();
            break;
        case "my_experts_questions" :
            $query = my_experts_questions();
            break;
        case "my_answered_questions" :
            return my_answered_questions($page);
            break;
        default:
            return "domain is not exist";
            break;
    }
    if($query != null){
        $query->orderBy('changed', 'DESC');
        $query->range($page*10, 10);
        $results = $query->execute()->fetchAll();
    }
    else return ["questions" => array()];
    if(empty($results)) return ["questions" => array()];

    $nids = array();
    $uids = array();
    $questions = array();
    foreach($results as $row){
        if($domain == "all_questions"){
            array_push($uids, $row->uid);
        }
        array_push($nids, $row->nid);
        $row->feild_experts = array();
        $row->field_files = array();
        $questions[$row->nid] = $row;
    }

    /*add each question's experts*/
    $results = add_experts($nids);
    foreach($results as $result){
        $tid = new stdClass();
        $tid->tid = $result->field_experts_tid;
        array_push($questions[$result->entity_id]->feild_experts, $tid);
    }

    /*add each question that how many experts answered it*/
    $results = number_of_experts_answered($nids);
    foreach($results as $result){
        $questions[$result->nid]->expert_answers = $result->expert_answers;
    }

    /*add each question if it has answer that is proved by advisor*/
    $results = answer_with_prove_by_advisor($nids);
    foreach($results as $result){
        $questions[$result->nid]->prove_advisor = 1;
    }

    /*add each question if it has answer that is proved by asker*/
    $results = answer_with_prove_by_asker($nids);
    foreach($results as $result){
        $questions[$result->nid]->prove_asker = 1;
    }

    /*add each question the count of answers*/
    $results = count_of_answers($nids);
    foreach($results as $result){
        $questions[$result->nid]->comment_count = $result->comment_count;
    }

    /*add each question it's file*/
    $results = files_of_question($nids);
    foreach($results as $result){
        array_push($questions[$result->entity_id]->field_files, $result);
    }

    $return_questions = array();
    foreach($questions as $q){
        array_push($return_questions, $q);
    }
    if($domain == "all_questions"){
        $users = user_information($uids);
        return ["questions" => $return_questions, "users" => $users];
    }
    return ["questions" => $return_questions];
}

function all_questions(){
    $query = db_select('node' , 'n');
    $query->leftJoin('field_data_body' , 'b' , 'n.nid = b.entity_id');
    $query->fields('n' , array('nid' , 'uid' , 'title' , 'created' , 'changed' , 'status'));
    $query->addField('b' , 'body_value' , 'body');
    $query->condition('n.type', 'questions');
    $query->condition('n.status', 1);
    return $query;
}

function my_questions(){
    global $user;
    $query = db_select('node' , 'n');
    $query->leftJoin('field_data_body' , 'b' , 'n.nid = b.entity_id');
    $query->fields('n' , array('nid' , 'title' , 'created' , 'changed' , 'status'));
    $query->addField('b' , 'body_value' , 'body');
    $query->condition('n.type', 'questions');
    $query->condition('n.uid', $user->uid);
    return $query;
}

function following_questions(){
    global $user;
    $query = db_select('node' , 'n');
    $query->join('flagging' , 'f' , 'n.nid = f.entity_id');
    $query->fields('n' , array('nid' , 'uid' , 'title' , 'created' , 'changed' , 'status'));
    $query->condition('n.type', 'questions');
    $query->condition('n.status', 1);
    $query->condition("f.fid" , 3);
    $query->condition("f.uid" , $user->uid);
    return $query;
}

function my_experts_questions(){
    global $user;
    $profile = profile2_load_by_user($user);
    $field_experts = $profile["main"]->field_experts['und'];
    $experts = array();
    foreach($field_experts as $e){
        array_push($experts, $e["tid"]);
    }
    if(!empty($experts)){
        $query = db_select('node' , 'n');
        $query->join('field_data_field_experts' , 'e' , 'n.nid = e.entity_id');
        $query->fields('n' , array('nid' , 'uid' , 'title' , 'created' , 'changed' , 'status'));
        $query->condition('n.type' , 'questions');
        $query->condition('n.status' , 1);
        $query->condition('e.field_experts_tid' , $experts, 'IN');
        $query->condition('e.entity_type' , 'node');
        return $query;
    }
    else return null;
}

function my_answered_questions($page){
    global $user;
    $query = db_select('comment' , 'comment');
    $query->join('node' , 'node', 'comment.nid = node.nid');
    $query->join('field_data_comment_body' , 'body', 'comment.cid = body.entity_id');
    $query->join('field_data_field_prove_advisor' , 'advisor', 'comment.cid = advisor.entity_id');
    $query->join('field_data_field_prove_asker' , 'asker', 'comment.cid = asker.entity_id');
    $query->fields('comment' , array('cid' , 'pid' , 'nid' , 'subject'));
    $query->addField('node' , 'title' , 'node_title');
    $query->addField('body' , 'comment_body_value' , 'body');
    $query->addField('advisor' , 'field_prove_advisor_value' , 'prove_by_advisor');
    $query->addField('asker' , 'field_prove_asker_value' , 'prove_by_asker');
    $query->condition('body.entity_type' , 'comment');
    $query->condition('body.bundle' , 'comment_node_questions');
    $query->condition('comment.uid' , $user->uid);
    $query->range($page*10 , 10);
    $comments = $query->execute()->fetchAll();
    return ["questions" => $comments];
}

function add_experts($nids){
    $query = db_select('field_data_field_experts' , 'e');
    $query->fields('e' , array('entity_id' , 'field_experts_tid'));
    $query->condition('e.entity_id', $nids, "IN");
    $results = $query->execute()->fetchAll();
    return $results;
}

function number_of_experts_answered($nids){
    $query = db_select('comment' , 'c');
    $query->join('users_roles' , 'r' , 'c.uid = r.uid');
    $query->fields('c' , array('nid'));
    $query->condition('c.nid', $nids, 'IN');
    $query->condition('r.rid', 10);
    $query->groupBy('c.nid');
    $query->addExpression('count(c.cid)', 'expert_answers');
    $results = $query->execute()->fetchAll();
    return $results;
}

function answer_with_prove_by_advisor($nids){
    $query = db_select('comment' , 'c');
    $query->join('field_data_field_prove_advisor', 'f' , 'c.cid = f.entity_id');
    $query->fields('c', array('nid'));
    $query->condition('c.nid', $nids, "IN");
    $query->condition('f.field_prove_advisor_value', 1);
    $query->groupBy('c.nid');
    $results = $query->execute()->fetchAll();
    return $results;
}

function answer_with_prove_by_asker($nids){
    $query = db_select('comment' , 'c');
    $query->join('field_data_field_prove_asker', 'f' , 'c.cid = f.entity_id');
    $query->fields('c', array('nid'));
    $query->condition('c.nid', $nids, "IN");
    $query->condition('f.field_prove_asker_value', 1);
    $query->groupBy('c.nid');
    $results = $query->execute()->fetchAll();
    return $results;
}

function count_of_answers($nids){
    $query = db_select('comment' , 'c');
    $query->fields('c', array('nid'));
    $query->condition('c.nid', $nids, 'IN');
    $query->groupBy('c.nid');
    $query->addExpression('count(c.cid)', 'comment_count');
    $results = $query->execute()->fetchAll();
    return $results;
}

function files_of_question($nids){
    $query = db_select('field_data_field_files' , 'feild_file');
    $query->join('file_managed' , 'file' , 'file.fid = feild_file.field_files_fid');
    $query->fields('feild_file', array('entity_id'));
    $query->fields('file', array('fid' , 'filename', 'uri' ,  'filemime' , 'filesize' , 'type' , 'timestamp'));
    $query->condition('feild_file.entity_id' , $nids , 'IN');
    $query->condition('feild_file.entity_type' , 'node');
    $query->condition('feild_file.bundle' , 'questions');
    $results = $query->execute()->fetchAll();
    return $results;
}

function retrieve_a_question_version_one($nid){
    $query = db_select('node' , 'n');
    $query->leftJoin('field_data_body' , 'b' , 'n.nid = b.entity_id');
    $query->fields('n' , array('nid' , 'uid' , 'title' , 'type' , 'created' , 'changed' , 'status'));
    $query->addField('b' , 'body_value' , 'body');
    $query->condition('n.nid', $nid);
    $node = $query->execute()->fetch();
    if(strcmp($node->type , "questions") != 0 || empty($node)) return "Wrong ID";

    $node->url = "http://civil808.com/node/".$nid;

    global $user;

    $query = db_select('field_data_field_experts' , 'e');
    $query->addField('e' , 'field_experts_tid' , 'tid');
    $query->condition('e.entity_id' , $nid);
    $tids = $query->execute()->fetchAll();
    $node->field_experts = $tids;

    $query = db_select('field_data_field_files' , 'feild_file');
    $query->join('file_managed' , 'file' , 'file.fid = feild_file.field_files_fid');
    $query->fields('file', array('fid' , 'filename', 'uri' ,  'filemime' , 'filesize' , 'type' , 'timestamp'));
    $query->condition('feild_file.entity_id' , $nid);
    $query->condition('feild_file.entity_type' , 'node');
    $query->condition('feild_file.bundle' , 'questions');
    $results = $query->execute()->fetchAll();
    if(!empty($results)) $node->field_files = $results;

    $query = db_select('flagging' , 'f')
        ->condition('fid' , 3)
        ->condition('entity_id' , $nid)
        ->condition('uid' , $user->uid);
    $count = $query->countQuery()->execute()->fetchField();

    if($count > 0) $node->follow_status = 1;
    else $node->follow_status = 0;

    $query = db_select('users' , 'u');
    $query->leftJoin('file_managed' , 'f' , 'u.picture = f.fid');
    $query->fields('u' , array('uid' , 'name' , 'mail' , 'created'));
    $query->addField('f' , 'uri' , 'picture');
    $query->condition('u.uid', $node->uid);
    $author = $query->execute()->fetch();

    $criteria = array();
    $criteria['entity_id'] = $node->nid;
    $criteria['entity_type'] = "node";
    $criteria['function'] = "sum";
    $vote_points = votingapi_select_results($criteria);
    $vote_points = $vote_points[0]['value'];

    $query = db_select('votingapi_vote', 'v');
    $query->fields('v' , array('value'));
    $query->condition('v.entity_type' , 'node');
    $query->condition('v.entity_id' , $node->nid);
    $query->condition('v.uid' , $user->uid);
    $user_vote = $query->execute()->fetchObject();

    $query = db_select('field_data_field_prove_advisor' , 'f');
    $query->join('comment' , 'c' , 'f.entity_id = c.cid');
    $query->fields('f' , array('entity_id'));
    $query->condition('c.nid', $nid);
    $query->condition('f.field_prove_advisor_value', 1);
    $result = $query->execute()->fetchAll();
    if(!empty($result) && count($result)>0)  $node->prove_advisor = 1;

    $query = db_select('field_data_field_prove_asker' , 'f');
    $query->join('comment' , 'c' , 'f.entity_id = c.cid');
    $query->fields('f' , array('entity_id'));
    $query->condition('c.nid', $nid);
    $query->condition('f.field_prove_asker_value', 1);
    $result = $query->execute()->fetchAll();
    if(!empty($result) && count($result)>0)  $node->prove_asker = 1;

    return ["question" => $node, "author" => $author, "vote_points" => $vote_points , "user_vote" => $user_vote->value];
}

function create_a_question_version_one($question){
    global $user;
    $newQuestion= new stdClass();
    $newQuestion->type  = "questions";
    $newQuestion->title  = $question['title'];
    $newQuestion->body['und'][0]['value'] = $question['body'];
    $newQuestion->body['und'][0]['summary'] = '';
    $newQuestion->body['und'][0]['format'] = 'full_html';
    $newQuestion->field_experts  = $question['field_experts'];
    if(isset($question['field_files']) && !empty($question['field_files'])) $newQuestion->field_files  = $question['field_files'];
    $newQuestion->uid  = $user->uid;
    $newQuestion->language = 'fa';
    $newQuestion->status = 0;
    $newQuestion->comment = 2;
    $newQuestion = node_submit($newQuestion);
    node_save($newQuestion);
    return $newQuestion;
}

function edit_a_questions_version_one($nid , $question){
    $newQuestion = node_load($nid);
    $newQuestion->title = $question['title'];
    $newQuestion->body['und'][0]['value'] = $question['body'];
    $newQuestion->field_experts = $question['field_experts'];
    if(isset($question['field_files']) && !empty($question['field_files'])) $newQuestion->field_files = $question['field_files'];
    else $newQuestion->field_files = [];
    node_save($newQuestion);
    return $newQuestion;
}

function follow_a_question_version_one($nid , $data){
    global $user;
    if(!isset($data['action']) && empty($data['action'])) return "error! action is empty";

    if(($data['action'] == "follow" || $data['action'] == 1) && follow_status($nid , $user->uid) == 0){
        $fid = db_insert('flagging')
            ->fields(array(
                'fid' => 3,
                'entity_type' => 'node',
                'entity_id' => $nid,
                'uid' => $user->uid,
                'sid' => 0,
                'timestamp' => strtotime(date("Y-m-d H:i:s", time()))
            ))
            ->execute();

        return $fid;
    }
    elseif(($data['action'] == "unfollow" || $data['action'] == 0) && follow_status($nid , $user->uid) == 1){
        $num_deleted = db_delete('flagging')
            ->condition('uid', $user->uid)
            ->condition('entity_id', $nid)
            ->execute();

        return $num_deleted;
    }
    return "error";
}

function follow_status($nid , $uid){
    $query = db_select('flagging' , 'f')
        ->condition('fid' , 3)
        ->condition('entity_id' , $nid)
        ->condition('uid' , $uid);
    $count = $query->countQuery()->execute()->fetchField();

    if($count > 0) return 1;
    else return 0;
}

function vote_a_question_version_one($nid , $data){
    global $user;
    if(!isset($data['action']) && empty($data['action'])) return "error! action is empty";

    if($data['action'] == "up") $value = 1;
    elseif($data['action'] == "down") $value = -1;
    else return "error";

    $votes = array();
    $votes['entity_type'] = "node";
    $votes['entity_id'] = $nid;
    $votes['value_type'] = "points";
    $votes['value'] = $value;
    $votes['tag'] = "vote";
    $votes['uid'] = $user->uid;
    votingapi_set_votes($votes);
    return $votes;
}